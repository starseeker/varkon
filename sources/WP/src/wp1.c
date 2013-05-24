/**********************************************************************
*
*    wp1.c
*    =====
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpinit();   Init WP
*    wpexit();   Close WP
*    wpclrg();   Close graphical windows
*    wpsvjb();   Write graphical window data to JOB-file
*    wpldjb();   Read graphical window data from jobfile
*    wpngws();   Return current number of graphical windows
*    wpcgws();   Create graphical windows
*    wpmrdb();   Merge X resource databases
*    wpgtwi();   Return window data
*    wpgtwp();   Return window position 
*    wpgwsz();   Return window size
*    wpsdpr();   Set window PROTOCOL
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
#include "../../EX/include/EX.h"
#include "../include/WP.h"
#include <string.h>


Window       xgwin;     /* För grapac */
Pixmap       xgmap;     /* För grapac */
GC           gpxgc;     /* För grapac */

Display     *xdisp;
GC           xgc;
XPoint       xbuf[PLYMXV];
int          xscr;
MNUDAT      *actmeny;
Cursor       xgcur1,xgcur2,xwcur,xtcur;
XrmDatabase  xresDB;

/* xgwin är window ID för det grafiska huvud-fönstret.
   xdisp är display ID.
   xgc är Graphic Context ID.
   xbuf är grapac:s polyline-buffert
   xscr är Screen,
   actmeny är en pekare till aktiv meny. Om meny saknas är actmeny = NULL.
   xgcur är den cursor som aktiveras vid hårkors-pekning.
   xtcur är I-balk cursorn vid textinmatning
   xwcur är vänta-klockan
   xresDB är den resursdatabas som skapas vid uppstart.
*/

extern short  gpsnpx,gpsnpy,gpgorx,gpgory,gpgnpx,gpgnpy;
extern double gpgszx,gpgszy;
extern VY     actvy;

/*!******************************************************/

        short wpinit(
        char *inifil_1,
        char *inifil_2)

/*      Öppnar winpac. Förutsätter att resursdatabasen
 *      är skapad av andra rutiner tidigare.
 *
 *      In: inifil_1 = Ev. ytterligare resursfil.
 *          inifil_2 = Ev. ytterligare resursfil.
 *
 *      Ut: Inget.
 *
 *      Felkod: WP1042 = Kan ej öppna dislay.
 *              WP1032 = Kan ej initiera fonter på display %s
 *              WP1252 = Kan ej skapa meny-fönster på %s
 *              WP1442 = Hittar ej resursfilen %s
 *
 *      (C)microform ab 23/6/92 U.Andersson
 *
 *      24/1/94  Omarbetad, J. Kjellander
 *      7/11/94  Mer resurser, J. Kjellander
 *      8/1/95   Multifönster, J. Kjellander
 *      21/12/95 Ytterligare resursfil, J. Kjellander
 *      1998-03-12 Ytterligare resursfil igen, J. Kjellander
 *
 ******************************************************!*/

  {
    short       status;
    XGCValues   values;
    XrmDatabase xdfDB;

/*
***Innan nåt annat görs mergar vi ev. initfiler med den redan
***skapade resursdatabasen.
*/
    if ( strlen(inifil_1) > 0  )
      {
      if ( v3ftst(inifil_1) )
        {
        xdfDB = XrmGetFileDatabase(inifil_1);
        XrmMergeDatabases(xdfDB,&xresDB);
        }
      else return(erpush("WP1442",inifil_1));
      }

    if ( strlen(inifil_2) > 0  )
      {
      if ( v3ftst(inifil_2) )
        {
        xdfDB = XrmGetFileDatabase(inifil_2);
        XrmMergeDatabases(xdfDB,&xresDB);
        }
      else return(erpush("WP1442",inifil_2));
      }
/*
***Öppna display.
*/
    if ( (xdisp=XOpenDisplay(NULL)) == NULL )
      return(erpush("WP1042",XDisplayName(NULL)));
/*
***Initiera V3:s fönsterhanterare.
*/
    wpwini();
/*
***Ta reda på skärm och antal bitplan.
*/
    xscr   = DefaultScreen(xdisp);
/*
***Initiera färger.
*/
    wpcini();
/*
***Skapa graphic context GC för winpac-fönster.
***Sätt graphics_exposures till False så att inte
***onödiga event typ GraphicsExpose eller NoExpose
***genereras i onödan.
*/
    xgc =  DefaultGC(xdisp,xscr);
    values.graphics_exposures = False;
    XChangeGC(xdisp,xgc,GCGraphicsExposures,&values);
    XSetBackground(xdisp,xgc,wpgcol(WP_BGND));
    XSetForeground(xdisp,xgc,wpgcol(WP_FGND));
/*
***Initiera text-fonter.
*/
    status = wpfini();
    if ( status < 0 ) return(erpush("WP1032",XDisplayName(NULL)));
/*
***Skapa grafik-cursor.
*/
    xgcur1 = XCreateFontCursor(xdisp,34);
    xgcur2 = XCreateFontCursor(xdisp,52);
    xwcur  = XCreateFontCursor(xdisp,150);
/*
***Initiera menyhanteraren.
*/
    if ( (status=wpmini()) < 0 ) return(status);
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpexit()

/*      Avslutar winpac.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 23/6/92 U.Andersson
 *
 ******************************************************!*/

  {
/*
***Nollställ allt som har med grafiska fönster att göra.
*/
   wpclrg();
/*
***Nollställ resten också.
*/
   wpwexi();
/*
***Inga menyer nu aktiva.
*/
   actmeny = NULL;
/*
***Stäng display.
*/
   XCloseDisplay(xdisp);
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpclrg()

/*      Dödar alla grafiska fönster och nollställer
 *      det som har med dem att göra.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 27/9/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int     i;
   WPWIN  *winptr;

/*
***Sudda meddelandefönstret så att dess button-pekare
***nollställs. Annars funkar inte lagra/nytt jobb.
*/
   wpwlma("");
/*
***Döda alla grafiska fönster.
*/
   for ( i=0; i<WTABSIZ; ++i )
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) wpwdel((v2int)i);
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpsvjb(
        FILE *f)

/*      Lagrar grafiska fönster i jobfilen.
 *
 *      In: f => Pekare till öppen jobbfil.
 *
 *      Ut: Inget.
 *
 *      Felkod: WP1422 = Fel vid skrivning till jobfil.
 *
 *      (C)microform ab 28/1/95 J. Kjellander
 *
 ******************************************************!*/

 {
   short   corrx,corry;
   int     i,winant;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Hur många WPGWIN har vi ?
*/
   winant = wpngws();

/*
***Skriv ut antal fönster.
*/
   if ( fwrite((char *)&winant,sizeof(int),1,f) == 0 ) goto error;
/*
***För varje fönster, skriv ut placering, storlek, vynamn, modell-
***fönster och nivåer.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN )
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       corrx = gwinpt->geo.x - gwinpt->wmandx;
       corry = gwinpt->geo.y - gwinpt->wmandy;
       if ( fwrite((char *)&corrx,sizeof(short),1,f) == 0 )
         goto error;
       if ( fwrite((char *)&corry,sizeof(short),1,f) == 0 )
         goto error;

       if ( fwrite((char *)&gwinpt->geo.dx,sizeof(short),1,f) == 0 )
         goto error;
       if ( fwrite((char *)&gwinpt->geo.dy,sizeof(short),1,f) == 0 )
         goto error;
       if ( fwrite((char *)&gwinpt->vy.vynamn,sizeof(char),GPVNLN+1,f) == 0 )
         goto error;
       if ( fwrite((char *)&gwinpt->vy.modwin,sizeof(VYWIN),1,f) == 0 )
         goto error;
       if ( fwrite((char *)gwinpt->nivtab,sizeof(char),WP_NTSIZ,f) == 0 )
         goto error;
       }
     }
/*
***Slut.
*/
   return(0);
/*
***Felutgång.
*/
error:
   return(erpush("WP1422",""));
 }

/********************************************************/
/*!******************************************************/

        short wpldjb(
        FILE *f)

/*      Laddar grafiska fönster från jobfilen.
 *
 *      In: f => Pekare till öppen jobbfil.
 *
 *      Ut: Inget.
 *
 *      Felkoder: WP1432 = Kan ej läsa jobfil.
 *                WP1242 = Kan ej skapa fönster
 *
 *      (C)microform ab 28/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int       i,winant,ix,iy,idx,idy; 
   short     status,x,y,dx,dy;
   v2int     w_id;
   WPGWIN   *gwinpt,tmpgwi;

/*
***Hur många fönster skall skapas ?
*/
   if ( fread(&winant,sizeof(int),1,f) == 0 ) goto error;
/*
***Om winant > 0 börjar vi med huvudfönstret.
*/
   if ( winant > 0 )
     {
     if ( fread(&x,sizeof(short),1,f) == 0 ) goto error;
     if ( fread(&y,sizeof(short),1,f) == 0 ) goto error;
     if ( fread(&dx,sizeof(short),1,f) == 0 ) goto error;
     if ( fread(&dy,sizeof(short),1,f) == 0 ) goto error;
     if ( fread(&tmpgwi.vy.vynamn,sizeof(char),GPVNLN+1,f) == 0 ) goto error;
     if ( fread(&tmpgwi.vy.modwin,sizeof(VYWIN),1,f) == 0 ) goto error;
     if ( fread( tmpgwi.nivtab,sizeof(char),WP_NTSIZ,f) == 0 ) goto error;
/*
***Kolla att resultatet hamnar på skärmen.
*/
     ix = x; iy = y; idx = dx; idy = dy;
     wpposw(ix,iy,idx+10,idy+25,&ix,&iy);
/*
***Skapa själva huvudfönstret.
*/
     status = wpwcgw((short)ix,(short)iy,(short)idx,(short)idy,"",TRUE,&w_id);
     if ( status < 0 ) return(erpush("WP1242",XDisplayName(NULL)));
/*
***Vilket X-id fick fönstret och dess 'save_under' pixmap ?
*/
     gwinpt  = (WPGWIN *)wpwtab[(wpw_id)w_id].ptr;
     xgwin   = gwinpt->id.x_id;
     xgmap   = gwinpt->savmap;
     gpxgc   = gwinpt->win_gc;
/*
***Underrätta grapac om resultatet.
*/
     gpsnpx = gwinpt->geo.dx;
     gpsnpy = gwinpt->geo.dy;
     gpgorx = gwinpt->vy.scrwin.xmin;
     gpgory = gwinpt->vy.scrwin.ymin;
     gpgnpx = (short)(gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
     gpgnpy = (short)(gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
     gpgszx = gpgnpx/(double)DisplayWidth(xdisp,xscr) *
                        XDisplayWidthMM(xdisp,xscr);
     gpgszy = gpgnpy/(double)DisplayHeight(xdisp,xscr) *
                        XDisplayHeightMM(xdisp,xscr);
/*
***Aktivera fönstrets vy. Detta gör vi genom att först aktivera
***den version av vyn som finns i vytab och sedan ändra modell-
***fönstret så att det stämmer med jobfilen. Normalisering behövs
***för att beräkna nya 2D-konstanter och för att kompensera för
***olika storlek på pixels i X- och Y-led på olika skärmar. Slutligen
***aktiverar vi vyn igen varvid såväl vytab som actvy får rätt
***utseende.
*/
     wpacvi(tmpgwi.vy.vynamn,GWIN_MAIN);
     V3MOME(&tmpgwi.vy.modwin,&gwinpt->vy.modwin,sizeof(VYWIN));
     wpnrgw(gwinpt);
     wpacvi(tmpgwi.vy.vynamn,GWIN_MAIN);
/*
***Kopiera den lästa nivåtabellen till det skapade fönstret.
*/
     V3MOME(tmpgwi.nivtab,gwinpt->nivtab,WP_NTSIZ);
     }
/*
***Eventuella ytterligare fönster.
*/
   for ( i=1; i<winant; ++i )
     {
     if ( fread(&x,               sizeof(short),      1,f) == 0 ) goto error;
     if ( fread(&y,               sizeof(short),      1,f) == 0 ) goto error;
     if ( fread(&dx,              sizeof(short),      1,f) == 0 ) goto error;
     if ( fread(&dy,              sizeof(short),      1,f) == 0 ) goto error;
     if ( fread(&tmpgwi.vy.vynamn,sizeof(char),GPVNLN+1,f) == 0 ) goto error;
     if ( fread(&tmpgwi.vy.modwin,sizeof(VYWIN),      1,f) == 0 ) goto error;
     if ( fread( tmpgwi.nivtab,   sizeof(char),WP_NTSIZ,f) == 0 ) goto error;

     ix = x; iy = y; idx = dx; idy = dy;
     wpposw(ix,iy,idx+10,idy+25,&ix,&iy);

     status = wpwcgw((short)ix,(short)iy,(short)idx,(short)idy,"",FALSE,&w_id);
     if ( status < 0 ) return(erpush("WP1242",XDisplayName(NULL)));

     gwinpt  = (WPGWIN *)wpwtab[(wpw_id)w_id].ptr;
     wpacvi(tmpgwi.vy.vynamn,w_id);
     V3MOME(&tmpgwi.vy.modwin,&gwinpt->vy.modwin,sizeof(VYWIN));
     wpnrgw(gwinpt);

     V3MOME(tmpgwi.nivtab,gwinpt->nivtab,WP_NTSIZ);
     }
/*
***Slut.
*/
    return(0);
/*
***Felutgång.
*/
error:
   return(erpush("WP1432",""));
  }

/********************************************************/
/*!******************************************************/

        int wpngws()

/*      Talar om hur många grafiska fönster som finns för
 *      tillfället.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Antal grafiska fönster.
 *
 *      (C)microform ab 28/1/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int     i,winant;
   WPWIN  *winptr;

/*
***Initiering.
*/
   winant = 0;
/*
***Hur många WPGWIN har vi ?
*/
   for ( i=0; i<WTABSIZ; ++i )
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) winant++;

   return(winant);
  }

/********************************************************/
/*!******************************************************/

        short wpcgws(
        bool create)

/*      Skapar grafiska fönster enligt resursdatabsen.
 *      Om create = TRUE skapas och aktiveras även default
 *      vy.
 *      Sätter div. grapac-variabler.
 *
 *      In: create => TRUE  = Skapa även default vy.
 *                    FALSE = Skapa bara fönstren.
 *
 *      Ut: Inget.   
 *
 *      Felkod: WP1242 = Kan ej skapa grafiskt fönster på %s
 *
 *      (C)microform ab 18/1-95 J. Kjellander
 *
 ******************************************************!*/

  {
   int       height,width,nadd,i,x,y; 
   unsigned int dx,dy;
   long      flags;
   char     *type[20];
   char      xrmstr1[81],xrmstr2[81],numstr[81];
   short     status;
   v2int     w_id;
   char      avynam[GPVNLN+1];
   XrmValue  value;
   WPGWIN   *gwinpt;
   VYVEC     bpos;

/*
***Eftersom wpwcgw() anropar wpupgp() kommer actvy att skrivas över.
***Spara namnet på aktiv vy så att denna kan aktiveras om jobb har
***laddats från jobfil utan fönster.
*/
   strcpy(avynam,actvy.vynamn);
/*
***V3:s grafiska huvud-fönster, GWIN_MAIN.
***Hårdprogrammerade defaultvärden.
*/
   width  = DisplayWidth(xdisp,xscr);
   height = DisplayHeight(xdisp,xscr); 

   x  = 0.2*width - 12;
   y  = 5;
   dx = 0.8*width - 15;
   dy = 0.9*height;
/*
***Värden från resursdatabasen.
***Kolla att resultatet hamnar på skärmen.
*/
   if ( XrmGetResource(xresDB,"varkon.grawin_1.geometry",
                              "Varkon.Grawin_1.Geometry",
                               type,&value) )
     {
     flags = XParseGeometry((char *)value.addr,&x,&y,&dx,&dy);
     if ( XValue & flags )
       if ( XNegative & flags ) x = width + x - dx;
     if ( YValue & flags )
       if ( YNegative & flags ) y = height + y - dy;
     }

   wpposw(x,y,dx+10,dy+25,&x,&y);
/*
***Skapa själva fönstret. Observera att detta är det första 
***fönstret som skapas och det får därmed ID = 0 = GWIN_MAIN.
***Så måste det ovillkorligen vara eftersom många rutiner inte
***minst i MBS bygger på detta.
*/
   status = wpwcgw((short)x,(short)y,(short)dx,(short)dy,"",TRUE,&w_id);
   if ( status < 0 ) return(erpush("WP1242",XDisplayName(NULL)));
/*
***Vilket X-id fick fönstret och dess 'save_under' pixmap ?
*/
   gwinpt  = (WPGWIN *)wpwtab[(wpw_id)w_id].ptr;
   xgwin   = gwinpt->id.x_id;
   xgmap   = gwinpt->savmap;
   gpxgc   = gwinpt->win_gc;
/*
***Hela skärmens storlek i pixels till grapac.
*/
   gpsnpx = gwinpt->geo.dx;
   gpsnpy = gwinpt->geo.dy;
/*
***Grafiska areans origo till grapac.
*/
   gpgorx = gwinpt->vy.scrwin.xmin;
   gpgory = gwinpt->vy.scrwin.ymin;
/*
***Grafiska areans storlek i pixels och mm. till grapac.
*/
   gpgnpx = (short)(gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   gpgnpy = (short)(gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);

   gpgszx = gpgnpx/(double)DisplayWidth(xdisp,xscr) *
                      XDisplayWidthMM(xdisp,xscr);
   gpgszy = gpgnpy/(double)DisplayHeight(xdisp,xscr) *
                      XDisplayHeightMM(xdisp,xscr);
/*
***Kanske skall vi också skapa default vy. Genom att först sätta
***aktiv vy:s modellfönster = Huvudfönstrets ser vi till att vyn
***"xy" får samma modellfönster som GWIN_MAIN. EXcrvp() kopierar
***nämligen sitt modellfönster från aktiv vy.
*/
   if ( create )
     {
     actvy.vywin[0] = gwinpt->vy.modwin.xmin;
     actvy.vywin[1] = gwinpt->vy.modwin.ymin;
     actvy.vywin[2] = gwinpt->vy.modwin.xmax;
     actvy.vywin[3] = gwinpt->vy.modwin.ymax;
     bpos.x_vy = 0.0;
     bpos.y_vy = 0.0;
     bpos.z_vy = 1.0;
     EXcrvp("xy",&bpos);
     wpacvi("xy",GWIN_MAIN);
     }
   else wpacvi(avynam,GWIN_MAIN);
/*
***Eventuella ytterligare grafiska fönster.
***Antal ytterligare är till att börja med = 0.
*/
   nadd = 0;

   for ( i=2; i<WP_GWMAX; ++i )
     {
     strcpy(xrmstr1,"varkon.grawin_");
     strcpy(xrmstr2,"Varkon.Grawin_");
     sprintf(numstr,"%d",i);
     strcat(xrmstr1,numstr);
     strcat(xrmstr2,numstr);
     strcat(xrmstr1,".geometry");
     strcat(xrmstr2,".Geometry");
/*
***Prova att hämta .geometry-resursen.
*/
     if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
       {
       flags = XParseGeometry((char *)value.addr,&x,&y,&dx,&dy);
       if ( XValue & flags )
         if ( XNegative & flags ) x = gwinpt->geo.dx + x - dx;
       if ( YValue & flags )
         if ( YNegative & flags ) y = gwinpt->geo.dy + y - dy;

       wpposw(x,y,dx+10,dy+25,&x,&y);
/*
***Sen skapar vi ett ytterligare grafiskt fönster.
*/
       status = wpwcgw((short)x,(short)y,(short)dx,(short)dy,"",FALSE,&w_id);
       if ( status < 0 ) return(erpush("WP1242",XDisplayName(NULL)));
/*
***Om create = TRUE skall vyn "xy" aktiveras i alla fönster.
*/
       if ( create ) wpacvi("xy",w_id);
       else          wpacvi(avynam,w_id);
/*
***Slutligen räknar vi upp nadd.
*/
       if ( ++nadd == WP_GWMAX ) return(0);
       }
     }
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpmrdb(
        int      *ac,
        char     *av[])

/*      Skapar resursdatabas. Anropas av main()->igppar() och
 *      lagrar pekare till datbasen i den globala variabeln
 *      xresDB.
 *
 *      In:  ac = Pekare till antal parametrar på kommandoraden.
 *           av = Kommandoradsparametrarna.
 *
 *      Ut:  Inget.
 *
 *      FV:  0.
 *
 *      (C)microform ab 8/2/94 J. Kjellander
 *
 *      940602 VMS, J. Kjellander
 *      1998-02-06 XPointer, J.Kjellander
 *
 ******************************************************!*/

  {
    char        path[256];
    XrmDatabase cmdDB,appDB,xdfDB;

#define OPTANT 10

static XrmOptionDescRec opttab[] = {
  {"-font",          ".font",        XrmoptionSepArg, (XPointer)NULL},
  {"-fn",            ".font",        XrmoptionSepArg, (XPointer)NULL},
  {"-geometry",      ".geometry",    XrmoptionSepArg, (XPointer)NULL},
  {"-title",         ".title",       XrmoptionSepArg, (XPointer)NULL},
  {"-bg",            ".background",  XrmoptionSepArg, (XPointer)NULL},
  {"-background",    ".background",  XrmoptionSepArg, (XPointer)NULL},
  {"-fg",            ".foreground",  XrmoptionSepArg, (XPointer)NULL},
  {"-foreground",    ".foreground",  XrmoptionSepArg, (XPointer)NULL},
  {"-iconic",        ".iconStartup", XrmoptionSepArg, (XPointer)"on"},
  {"-xrm",           NULL,           XrmoptionResArg, (XPointer)NULL},
 };

/*
***OPTANT anger hur många options som finns i opttab. opttab
***beskriver de optioner som supportas för närvarande.
*/

/*
***Ev. DEBUG. eftersom denna rutin anropas innan kommandoraden
***tolkats av V3 kan inte debug vara påslaget. En variant är
***att slå på debug här ovillkorligt. Av någon orsak verkar
***åtminstone SCO/UNIX-versionen av XrmParseCommand() strippa
***parametrar med bara 1 bokstav samt även följande parameter
***trots att de inte har något med X-resurser att göra. Ex. -g
***försvinner liksom -x. Lösningen är att ha dem sist eller att 
***inte använda parametrar med bara 1 bokstav. Möjligen har det 
***med ett mellanslag mellan parametern och värdet att göra.
***X tror att -g -gm50 är en tilldelning av parametern -g med
***värdet -gm50 eller nåt sånt. Förstår ändå inte riktigt.....
*/
/*
***Initiering.
*/
    XrmInitialize();
/*
***Skapa resursdatabas av ev. resursfil på "app-defaults".
*/
#ifdef UNIX
    appDB = XrmGetFileDatabase("/usr/lib/X11/app-defaults/Varkon");
#endif

#ifdef VMS
    appDB = XrmGetFileDatabase("V3$APP-DEFAULTS:Varkon");
#endif

    XrmMergeDatabases(appDB,&xresDB);
/*
***Merga med ev. ".Xdefaults" på i första hand "XENVIRONMENT"-
***directoryt och i andra hand "HOME"-directoryt.
*/
#ifdef UNIX
    if ( gtenv3("XENVIRONMENT") != NULL )
      strcpy(path,gtenv3("XENVIRONMENT"));
    else
      {
      strcpy(path,gtenv3("HOME"));
      strcat(path,"/.Xdefaults");
      }

    xdfDB = XrmGetFileDatabase(path);
#endif

#ifdef VMS
    xdfDB = XrmGetFileDatabase("SYS$LOGIN:.Xdefaults");
#endif

    XrmMergeDatabases(xdfDB,&xresDB);
/*
***Merga med ev. parametrar på kommandoraden.
***Parametrar som inte svarar mot opttab lämnas kvar och
***parsas av V3 självt. ac och av uppdateras isåfall av
***XrmParseCommand(). Som klassnamn använder vi "varkon".
*/
    cmdDB = NULL;
    XrmParseCommand(&cmdDB,(XrmOptionDescList)opttab,
                                     OPTANT,"varkon",ac,av);
    XrmMergeDatabases(cmdDB,&xresDB);
/*
***Slut.
*/
    return(0);

  }

/********************************************************/
/*!******************************************************/

        short  wpgtwi(
        v2int  id,
        int   *px,
        int   *py,
        int   *pdx,
        int   *pdy,
        int   *ptyp,
        char  *rubrik)

/*      Returnerar div. data om ett fönster.
 *
 *      In: id      = Fönster-ID.
 *
 *      Ut: *px     = Läge i X-led.
 *          *py     = Läge i Y-led.
 *          *pdx    = Storlek i X-led.
 *          *pdy    = Storlek i Y-led.
 *          *ptyp   = 0=Inmatningsfönster, 1=Grafiskt fönster.
 *          *rubrik = Fönstertitel.
 *
 *      Felkoder:
 *                WP1562 = Fönster %s finns ej
 *                WP1572 = Fönstret ej IWIN/GWIN.
 *
 *      (C)microform ab 1996-05-21 J. Kjellander
 *
 ******************************************************!*/

  {
    char    errbuf[80],*name;
    Window  xid;
    WPWIN  *winptr;
    WPIWIN *iwinpt;
    WPGWIN *gwinpt;

/*
***Är det möjligen hela skärmen som avses ?
*/
    if ( id == -1 )
      {
     *px   = *py = 0;
     *pdx  = DisplayWidth(xdisp,xscr);
     *pdy  = DisplayHeight(xdisp,xscr);
     *ptyp = -1;
      sprintf(rubrik,"%s:%d:%d:%d",
              ServerVendor(xdisp),VendorRelease(xdisp),
              ProtocolVersion(xdisp),ProtocolRevision(xdisp)); 
      return(0);
      }
/*
***Fixa C-pekare till fönstret.
*/
    if ( (winptr=wpwgwp(id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)id);
      return(erpush("WP1562",errbuf));
      }

    switch ( winptr->typ )
      {
      case TYP_IWIN:
      iwinpt = (WPIWIN *)winptr->ptr;
      xid    = iwinpt->id.x_id;
     *ptyp   = 0;
      break;

      case TYP_GWIN:
      gwinpt = (WPGWIN *)winptr->ptr;
      xid    = gwinpt->id.x_id;
     *ptyp   = 1;
      break;

      default:
      sprintf(errbuf,"%d",(int)id);
      return(erpush("WP1572",errbuf));
      }
/*
***Rubrik.
*/
   XFetchName(xdisp,xid,&name);

   if ( name != NULL )
     {
     strcpy(rubrik,name);
     XFree(name);
     }
   else strcpy(rubrik,"");
/*
***Placering och storlek.
*/
    wpgwsz(xid,px,py,pdx,pdy,NULL,NULL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpgtwp(
        Window  wid,
        int    *px,
        int    *py)

/*      Returnerar fönstrets nuvarande läge.
 *
 *      In: wid = Fönster-ID.
 *          px  = Pekare till läge i X-led.
 *          py  = Pekare till läge i Y-led.
 *
 *      Ut: *px  = Läge i X-led.
 *          *py  = Läge i Y-led.
 *
 *      FV:  0.
 *
 *      (C)microform ab 11/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    Window root,child;
    int    x,y;
    unsigned int dx,dy,bw,d;

    XGetGeometry(xdisp,wid,&root,&x,&y,&dx,&dy,&bw,&d);
    root = DefaultRootWindow(xdisp);
    XTranslateCoordinates(xdisp,wid,root,x,y,px,py,&child);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpgwsz(
        Window  wid,
        int    *px,
        int    *py,
        int    *pxp,
        int    *pyp,
        double *pxm,
        double *pym)

/*      Returnerar fönstrets nuvarande läge och storlek i antal
 *      pixels och i mm. Om fönstret är ett "Top Level"-fönster
 *      och ägs av fönsterhanteraren returneras x,y = 0.
 *
 *      In: wid = Fönster-ID.
 *
 *      Ut: *px  = Läge i X-led.
 *          *py  = Läge i Y-led.
 *          *pxp = Antal pixels i X-led.
 *          *pyp = Antal pixels i Y-led.
 *          *pxm = Antal millimeter i X-led.
 *          *pym = Antal millimeter i Y-led.
 *
 *      FV:  0.
 *
 *      (C)microform ab 29/6/92 J. Kjellander
 *
 ******************************************************!*/

  {
    Window root,child;
    unsigned int bw,d;

/*
***Fråga X efter storlek och placering i pixels.
***Efter många svåra stunder har det framkommit att
***fönstrets läge inte är relativt skärmen utan något
***annat, gud vet vad. px och py är alltid 0 efter 
***XGetGeometry. XTranslateCoordinates fixar detta dock.
***Fönster som är "top level" och ägs av motif verkar få
***0,0 som position.
*/
    XGetGeometry(xdisp,wid,&root,px,py,
                (unsigned int *)pxp,(unsigned int *)pyp,&bw,&d);
    XTranslateCoordinates(xdisp,wid,root,0,0,px,py,&child);
/*
***Vad blir det i millimeter ?
*/
    if ( pxm != NULL )
      *pxm = *pxp/(double)DisplayWidth(xdisp,xscr)*
                          XDisplayWidthMM(xdisp,xscr);

    if ( pym != NULL )
      *pym = *pyp/(double)DisplayHeight(xdisp,xscr)*
                          XDisplayHeightMM(xdisp,xscr);
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpsdpr(
        Window win)

/*      Aktiverar WM_PROTOCOLS med WM_DELETE_WINDOW
 *      för ett visst fönster. Detta får till effekt
 *      att en fönsterhanterare inte gör destroy på
 *      fönstret eller dödar processen när användaren
 *      väljer close i fönster-menyn. Istället
 *      skickas ett ClientMessageEvent till event-kön.
 *      ClientMessageEvents behöver man inte be om med
 *      XSelectInput, dessa kommer ändå!!!
 *
 *      Detta property skall bara användas på top-level
 *      fönster som ägs av fönsterhanteraren, dvs. det
 *      grafiska fönstret, listfönster etc. Rutinen skall
 *      anropas direkt efter att fönstret har skapats och
 *      innan det görs map på det.
 *
 *      In: win => Fönster-id.   
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 3/8/92 J. Kjellander
 *
 ******************************************************!*/

  {
    Atom   proto,delete,type;

/*
***Hämta Atoms.
*/
    proto  = XInternAtom(xdisp,"WM_PROTOCOLS",False);
    delete = XInternAtom(xdisp,"WM_DELETE_WINDOW",False);
    type   = XInternAtom(xdisp,"ATOM",False);
/*
***Aktivera WM_PROTOCOLS-property med data = WM_DELETE_WINDOW.
*/
    XChangeProperty(xdisp,win,proto,type,32,
                    PropModeReplace,(unsigned char *)&delete,1);

    return(0);
  }

/********************************************************/
