/**********************************************************************
*
*    ms4.c
*    =====
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     msfini();   Init fonts
*     msgfnr();   Font name to font number
*     msgfnt();   Font number to font handle
*     msfexi();   Returns font resources
*
*     mscini();   Inits colors
*     mscexi();   Returns colour resources
*     msccol();   Allocates colour
*     msgcol();   Colour number to pen handle
*     msgbrush(); Colour number to brush handle
*     msgrgb();   Colour number to COLORREF
*
*     msstrl();   Length of string with font 0
*     msgtsl();   Length of string with named font
*     msstrh();   Height of font 0
*     msgtsh();   Height of named font
*     msftpy();   Calculates text Y-ccordinate
*     mswstr();   Write text
*     msdivs();   Split text
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
*    (C) 1984-1999, Johan Kjellander, Microform AB
*    (C) 200-2004, Johan Kjellander, Örebro university
*
***********************************************************************/

#include "../../../sources/DB/include/DB.h"
#include "../../../sources/IG/include/IG.h"
#include "../../../sources/WP/include/WP.h"

static int CALLBACK enumcb();

#define FNTTBSIZ 20

typedef struct wpfont
{
char   size[V3STRLEN+1];
HFONT  hndl;
}WPFONT;

static short  actfnt;
static WPFONT fnttab[FNTTBSIZ];

/* fnttab är en tabell med information om laddade fonter.
   Alla namn initieras till "" och alla HFONT:s till NULL.
   actfnt håller reda på vilken font i fnttab som är aktiv
   dvs. ingår i aktiv DC. Font nummer 0 laddas som default
   font av msfini() vid uppstart. Övriga fonter laddas då
   olika typer av fönster, knappar, ikoner etc. skapas av V3
   självt eller via MBS.
*/


HPEN ms_black,ms_darkg,ms_normg,ms_liteg,ms_white;

/* Färger som används för att måla fönsterramar. */

typedef struct wpcolor
{
HPEN   p_handle;
HBRUSH b_handle;
bool   defined;
} WPCOLOR;

static WPCOLOR coltab[WP_NPENS];

/* coltab är en tabell med pen- och brush-handtag för normala
   färger. Varje entry i coltab svarar mot en färg med
   samma nummer. coltab initieras av mscini() vid uppstart.
*/

extern HFONT ms_lfont;

/*!******************************************************/

        int msfini()

/*      Init-rutin för font-hanteringen. Anropas av
 *      msinit() vid uppstart.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 7/11/95 J. Kjellander
 *
 *      1996-05-22 Färdigställd, J.Kjellander
 *    
 ******************************************************!*/

  {
   int  i;
   char sizbuf[V3STRLEN];

/*
***Nollställ fnttab.
*/
   for ( i=0; i<FNTTBSIZ; ++i )
     {
     fnttab[i].size[0] = '\0';
     fnttab[i].hndl    = NULL;
     }

/*
   HDC  dc;

   dc = GetDC(ms_main);

   EnumFontFamilies(dc,NULL,enumcb,0);
   EnumFontFamilies(dc,"Marlett",enumcb,0);
   EnumFontFamilies(dc,"Arial",enumcb,0);
   EnumFontFamilies(dc,"Courier New",enumcb,0);
   EnumFontFamilies(dc,"Times New Roman",enumcb,0);
   EnumFontFamilies(dc,"Algerian",enumcb,0);
   EnumFontFamilies(dc,"MT Extra",enumcb,0);
   EnumFontFamilies(dc,"MS LineDraw",enumcb,0);

   ReleaseDC(ms_main,dc);
*/

/*
***Skapa font nummer 0. Alla fonter i WIN32-versionen av VARKON
***har samma namn men kan ha olika storlek. Namnet tas alltid från
***INI-filen och för font 0 tar vi även storleken från INI-filen.
*/
   if ( !msgrst("FONT_SIZE",sizbuf) ) strcpy(sizbuf,"10");

   if ( msgfnr(sizbuf) < 0 ) return(erpush("WP1002",sizbuf));
/*
***Tills vidare för listfönster.
*/
   ms_lfont = CreateFont(-10,0,0,0,FW_EXTRALIGHT,FALSE,FALSE,FALSE,ANSI_CHARSET,
                         OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                         FIXED_PITCH | FF_DONTCARE,"Fixedsys");

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int   msgfnr(fntsiz)
        char *fntsiz;

/*      Letar upp en font med viss storlek i fnttab och
 *      returnerar dess nummer. Om fonten inte finns
 *      laddas (skaps) den och lagras i fnttab.
 *
 *      In: fntsiz = Fontstorlek.
 *
 *      Ut: Inget.
 *
 *      FV: fontnummer.
 *
 *      (C)microform ab 1996-05-22 J. Kjellander
 *    
 ******************************************************!*/

  {
   int   i,size;
   char  fntnam[V3STRLEN];
   HFONT fonth;

/*
***Finns fonten i fnttab ?
*/
   for ( i=0; i<FNTTBSIZ; ++i )
      if ( strcmp(fnttab[i].size,fntsiz) == 0 ) return(i);
/*
***Ej laddad. då gör vi det nu. Leta upp ledig plats i fnttab.
*/
   for ( i=0; i<FNTTBSIZ; ++i ) if ( fnttab[i].hndl == NULL ) break;
   if ( i == FNTTBSIZ ) return(erpush("WP1012",fntsiz));
/*
***Fontstorlek.
*/
   if ( sscanf(fntsiz,"%d",&size) == 1 ) size = -size;
   else size = -10;
/*
***Fontnamn.
*/
   if ( !msgrst("FONT_NAME",fntnam) ) strcpy(fntnam,"Fixedsys");
/*
***Ladda.
*/
   fonth = CreateFont(size,0,0,0,FW_EXTRALIGHT,FALSE,FALSE,FALSE,ANSI_CHARSET,
                         OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                         DEFAULT_PITCH | FF_DONTCARE,fntnam);
/*
***Lagra i fnttab.
*/
   strcpy(fnttab[i].size,fntsiz);
   fnttab[i].hndl = fonth;

   return(i);
  }

/********************************************************/
/*!******************************************************/

        HFONT msgfnt(fntnum)
        int   fntnum;

/*      Returnerar WIN32-fonthandtag till font med visst
 *      nummer.
 *
 *      In: fntnum = Fontnummer.
 *
 *      Ut: Inget.
 *
 *      FV: X-Fontpekare.
 *
 *      (C)microform ab 1996-05-22 J. Kjellander
 *
 *
 ******************************************************!*/

  {
    return(fnttab[fntnum].hndl);
  }

/********************************************************/
/*!******************************************************/

static int CALLBACK enumcb(lf,tm,ft,nf)
           ENUMLOGFONT   *lf;
           NEWTEXTMETRIC *tm;
           int            ft;
           LPARAM         nf;

/*      Callback-rutin för msfini(). Anropas 1 gång för
 *      varje tillgänglig font.
 *
 *      In: lf = Pekare till data för logisk font.
 *          tm = Pekare till data för fysisk font.
 *          ft = Fonttyp.
 *          nf = Extra data, pekare till antal.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 7/11/95 J. Kjellander
 *    
 ******************************************************!*/

  {
  #ifdef DEBUG
     static nfonts = 0;

     if ( dbglev(WINPAC) == 4 )
       {
       fprintf(dbgfil(WINPAC),"%d, Style=%s, Name=%s\n",
         nfonts++,lf->elfStyle,lf->elfFullName);
       fflush(dbgfil(WINPAC));
       }
  #endif

    return(1);
  }

/********************************************************/
/*!******************************************************/

        int msfexi()

/*      Exit-rutin för font-hanteringen. Anropas av
 *      msexit() vid avslut.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 5/12/95 J. Kjellander
 *    
 ******************************************************!*/

  {

   DeleteObject(ms_lfont);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int mscini()

/*      Init-rutin för färg-hanteringen. Anropas av
 *      msinit() vid uppstart.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod:
 *
 *      (C)microform ab 12/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int i;
/*
***Initiera de 5 grånyanser som behövs för att måla fönsterramar.
***Används av msrpmw().
*/
   ms_black = CreatePen(PS_SOLID,1,RGB(  0,  0,  0));
   ms_darkg = CreatePen(PS_SOLID,1,RGB(128,128,128));
   ms_normg = CreatePen(PS_SOLID,1,RGB(192,192,192));
   ms_liteg = CreatePen(PS_SOLID,1,RGB(225,225,225));
   ms_white = CreatePen(PS_SOLID,1,RGB(255,255,255));
/*
***Initiera coltab.
*/
   for ( i=0; i<WP_NPENS; ++i) coltab[i].defined = FALSE;
/*
***Färg 0 = Vitt.
***     1 = Svart.
***     2 = Röd.
***     3 = Grön.
***     4 = Blå.
***     5 = Brun.
***     6 = Grå.
***     7 = Ljusblå.
***     8 = Orange.
***     9 = Gul.
***    10 = Violett.
***    11 = Turkos.
*/
   coltab[0].p_handle  = CreatePen(PS_SOLID,1,RGB(255,255,255));
   coltab[0].b_handle  = CreateSolidBrush(RGB(255,255,255));
   coltab[0].defined = TRUE;

   coltab[1].p_handle  = CreatePen(PS_SOLID,1,RGB(0,0,0));
   coltab[1].b_handle  = CreateSolidBrush(RGB(0,0,0));
   coltab[1].defined = TRUE;

   coltab[2].p_handle  = CreatePen(PS_SOLID,1,RGB(255,0,0));
   coltab[2].b_handle  = CreateSolidBrush(RGB(255,0,0));
   coltab[2].defined = TRUE;

   coltab[3].p_handle  = CreatePen(PS_SOLID,1,RGB(0,255,0));
   coltab[3].b_handle  = CreateSolidBrush(RGB(0,255,0));
   coltab[3].defined = TRUE;

   coltab[4].p_handle  = CreatePen(PS_SOLID,1,RGB(0,0,255));
   coltab[4].b_handle  = CreateSolidBrush(RGB(0,0,255));
   coltab[4].defined = TRUE;

   coltab[5].p_handle  = CreatePen(PS_SOLID,1,RGB(127,0,0));
   coltab[5].b_handle  = CreateSolidBrush(RGB(127,0,0));
   coltab[5].defined = TRUE;

   coltab[6].p_handle  = CreatePen(PS_SOLID,1,RGB(0,127,0));
   coltab[6].b_handle  = CreateSolidBrush(RGB(0,127,0));
   coltab[6].defined = TRUE;

   coltab[7].p_handle  = CreatePen(PS_SOLID,1,RGB(0,0,127));
   coltab[7].b_handle  = CreateSolidBrush(RGB(0,0,127));
   coltab[7].defined = TRUE;

   coltab[8].p_handle  = CreatePen(PS_SOLID,1,RGB(255,127,0));
   coltab[8].b_handle  = CreateSolidBrush(RGB(255,127,0));
   coltab[8].defined = TRUE;

   coltab[9].p_handle  = CreatePen(PS_SOLID,1,RGB(255,255,0));
   coltab[9].b_handle  = CreateSolidBrush(RGB(255,255,0));
   coltab[9].defined = TRUE;
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        int mscexi()

/*      Exit-rutin för färg-hanteringen. Anropas av
 *      msexit().
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod:
 *
 *      (C)microform ab 12/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int i;

/*
***Lämna tillbaks systemfärgerna.
*/
   DeleteObject(ms_black);
   DeleteObject(ms_darkg);
   DeleteObject(ms_normg);
   DeleteObject(ms_liteg);
   DeleteObject(ms_white);
/*
***Lämna tillbaks COLTAB-färgerna.
*/
   for ( i=0; i<WP_NPENS; ++i )
     {
     if ( coltab[i].defined )
       {
       DeleteObject(coltab[i].p_handle);
       DeleteObject(coltab[i].b_handle);
       }
     }
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        int msccol(pen,red,green,blue)
        int pen,red,green,blue;

/*      Allokerar en färg med givna RGB-värden till
 *      viss penna.
 *
 *      In: pen = Pennummer (färgnummer).
 *          red,green,blu = RGB-värden 0 -> 255.
 *
 *      Ut: Inget.
 *
 *      Felkoder:
 *
 *      (C)microform ab 1997-02-24 J. Kjellander
 *
 ******************************************************!*/

  {
    
/*
***Lite felkontroll.
*/
   if ( pen < 0 ) pen = 0;
   if ( pen > WP_NPENS-1 ) pen = WP_NPENS-1;

   if ( red   < 0 ) red   = 0;
   if ( green < 0 ) green = 0;
   if ( blue  < 0 ) blue  = 0;

   if ( red   > 255 ) red   = 255;
   if ( green > 255 ) green = 255;
   if ( blue  > 255 ) blue  = 255;
/*
***Skapa/allokera färgen.
*/
   coltab[pen].b_handle=CreateSolidBrush(RGB((BYTE)red,(BYTE)green,(BYTE)blue));
   if ( (coltab[pen].p_handle=CreatePen(PS_SOLID,1,
                                      RGB((BYTE)red,(BYTE)green,(BYTE)blue))) != NULL )
   coltab[pen].defined = TRUE;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        HPEN msgcol(colnum)
        int  colnum;

/*      Returnerar handtag till penna med viss färg.
 *      Om pennan inte har nån färg definierad i coltab[]
 *      returneras svart.
 *
 *      In: colnum = Färgnummer.
 *
 *      Ut: Inget.
 *
 *      FV: Penn-handtag.
 *
 *      (C)microform ab 12/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
    if ( colnum < 0 ) colnum = 0;
    if ( colnum > WP_NPENS-1 ) colnum = WP_NPENS - 1;

    if ( coltab[colnum].defined ) return(coltab[colnum].p_handle);
    else return(coltab[1].p_handle);
  }

/********************************************************/
/*!******************************************************/

        HBRUSH msgbrush(colnum)
        int  colnum;

/*      Returnerar handtag till pensel med viss färg.
 *
 *      In: colnum = Färgnummer.
 *
 *      Ut: Inget.
 *
 *      FV: Pensel-handtag.
 *
 *      (C)microform ab 1999-12-13 J. Kjellander
 *
 ******************************************************!*/

  {
    if ( colnum < 0 ) colnum = 0;
    if ( colnum > WP_NPENS-1 ) colnum = WP_NPENS - 1;

    if ( coltab[colnum].defined ) return(coltab[colnum].b_handle);
    else return(coltab[1].b_handle);
  }

/********************************************************/
/*!******************************************************/

        int       msgrgb(colnum,pcolor)
        int       colnum;
        COLORREF *pcolor;

/*      Returnerar COLORREF för penna med visst nummer.
 *
 *      In: colnum = Färgnummer.
 *          pcolor = Pekare till utdata
 *
 *      Ut: *pcolor = COLORREF-värde
 *
 *      (C)microform ab 1997-04-13 J. Kjellander
 *
 ******************************************************!*/

  {
   LOGPEN lpen;

/*
***Kolla att indata är rimliga.
*/
   if ( colnum < 0 ) colnum = 0;
   if ( colnum > WP_NPENS-1 ) colnum = WP_NPENS - 1;
/*
***Om färgen finns, returnera färgvärde.
*/
   if ( coltab[colnum].defined )
     {
     if ( GetObject(coltab[colnum].p_handle,sizeof(lpen),&lpen) > 0 ) *pcolor = lpen.lopnColor;
     else                                                             *pcolor = RGB(0,0,0);
     }
/*
***Annars returnera svart.
*/
   else *pcolor = RGB(0,0,0);
/*
***Slut.
*/
   return(0);   
  }

/********************************************************/
/*!******************************************************/

        int msstrl(str)
        char *str;

/*      Returnerar längden på en sträng i pixels om den
 *      skulle skrivas ut med font 0.
 *
 *      In: str = Sträng.
 *
 *      FV: Längden på strängen i pixels.
 *
 *      (C)microform ab 24/10/95 J. Kjellander
 *
 ******************************************************!*/

  {
  HDC  dc;
  SIZE size;

  dc = GetDC(ms_main);
  SelectObject(dc,msgfnt(0));
  GetTextExtentPoint32(dc,str,strlen(str),&size);
  ReleaseDC(ms_main,dc);

  return((int)size.cx);
  }

/********************************************************/
/*!******************************************************/

        int   msgtsl(str,font,plen)
        char *str;
        char *font;
        int  *plen;

/*      Räknar ut längden på en sträng i pixels om den
 *      skulle skrivas ut med font med visst namn.
 *
 *      In: str  = Text att skriva ut.
 *          font = Fontens namn.
 *
 *      Ut: *plen = Längd i pixels.
 *
 *      (C)microform ab 1996-05-21 J.Kjellander
 *
 ******************************************************!*/

  {
   int  fntnum;
   HDC  dc;
   SIZE size;

/*
***Om inget fontnamn getts tar vi font 0.
*/
   if ( *font == '\0' ) fntnum = 0;
/*
***Annars tar vi den från fonttabellen.
*/
   else fntnum = msgfnr(font);
/*
***Beräkna textlängden med motsvarande font.
*/
   dc = GetDC(ms_main);
   SelectObject(dc,msgfnt(fntnum));
   GetTextExtentPoint32(dc,str,strlen(str),&size);
   ReleaseDC(ms_main,dc);

  *plen = (int)size.cx;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int msstrh()

/*      Returnerar höjden på font 0.
 *
 *      In: Inget.
 *
 *      FV: Höjden på strängen i pixels.
 *
 *      (C)microform ab 24/10/95 J. Kjellander
 *
 ******************************************************!*/

  {
  SIZE size;
  HDC  dc;

  dc = GetDC(ms_main);
  SelectObject(dc,msgfnt(0));
  GetTextExtentPoint32(dc,"A",1,&size);
  ReleaseDC(ms_main,dc);

  return((int)size.cy + 2);
  }

/********************************************************/
/*!******************************************************/

        int   msgtsh(font,phgt)
        char *font;
        int  *phgt;

/*      Beräknar höjden på font med visst namn.
 *
 *      In: font = Fontens namn.
 *
 *      Ut: *phgt = Höjd i pixels.
 *
 *      (C)microform ab 1996-05-21 J.Kjellander
 *
 ******************************************************!*/

  {
  int  fntnum;
  SIZE size;
  HDC  dc;

/*
***Om inget fontnamn getts tar vi font 0.
*/
   if ( *font == '\0' ) fntnum = 0;
/*
***Annars tar vi den från fonttabellen.
*/
   else fntnum = msgfnr(font);
/*
***Beräkna höjden av "A" med motsvarande font.
*/
   dc = GetDC(ms_main);
   SelectObject(dc,msgfnt(fntnum));
   GetTextExtentPoint32(dc,"A",1,&size);
   ReleaseDC(ms_main,dc);

  *phgt = (int)size.cy + 2;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int msftpy(dy)
        int dy;

/*      Räknar ut y-koordinaten för en text i pixels.
 *
 *      In: 
 *          dy = Höjden för textfönstret.
 *
 *      Ut: Inget.
 *
 *      FV: Y-koordinaten i pixels.
 *
 *      (C)microform ab 24/10/95 J. Kjellander
 *
 *
 ******************************************************!*/

  {
  double     y;
  TEXTMETRIC tm;
  HDC        dc;

/*
typedef struct _TEXTMETRIC {

    LONG tmHeight;
    LONG tmAscent;
    LONG tmDescent;
    LONG tmInternalLeading;
    LONG tmExternalLeading;
    LONG tmAveCharWidth;
    LONG tmMaxCharWidth;
    LONG tmWeight;
    LONG tmOverhang;
    LONG tmDigitizedAspectX;
    LONG tmDigitizedAspectY;
    BCHAR tmFirstChar;
    BCHAR tmLastChar;
    BCHAR tmDefaultChar;
    BCHAR tmBreakChar;
    BYTE tmItalic;
    BYTE tmUnderlined;
    BYTE tmStruckOut;
    BYTE tmPitchAndFamily;
    BYTE tmCharSet;
} TEXTMETRIC;
*/
/*
***Ta reda på textfontens geometri.
*/
  dc = GetDC(ms_main);
  SelectObject(dc,msgfnt(0));
  GetTextMetrics(dc,&tm);
  ReleaseDC(ms_main,dc);
/*
***Avrunda uppåt.
*/
  
  y = dy - tm.tmDescent + tm.tmAscent - tm.tmInternalLeading;
  y = y/2.0 - 1;
  
  return((int)floor(y));
  }

/********************************************************/
/*!******************************************************/

        int   mswstr(dc,x,y,s)
        HDC   dc;
        int   x,y;
        char *s;

/*      Lågnivå-rutin för utskrift av text i fönster.
 *      Textfont, färg mm. ställs in innan denna rutin
 *      anropas.
 *
 *      In: dc  = Aktuellt DC.
 *          x,y = Position relativt fönstret.
 *          s   = Pekare till NULL-terminerad sträng.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 24/10/95 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Text placeras relativt baslinjen.
*/
   SetTextAlign(dc,TA_BASELINE);
/*
***Skriv ut.
*/
   TextOut(dc,x,y,s,strlen(s));

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int  msdivs(text,maxpix,tdx,tdy,str1,str2)
        char text[];
        int  maxpix,*tdx,*tdy;
        char str1[],str2[];

/*      Delar en textsträng i två delar.
 *      Beräknar platsbehov i pixels (x-och y-led)
 *
 *      In: text   = textsträng som ska användas.
 *          maxpix = max antal pixels som det aktiva fönstret innehåller.
 *
 *      Ut: tdx  = textbredd i pixels.
 *          tdy  = texthöjd i pixels.
 *          str1 = textsträng1.
 *          str2 = textsträng2. 
 *
 *      FV: 0.
 *
 *      (C)microform ab 24/10/95 J. Kjellander
 *
 ******************************************************!*/

  {
    int sl,slm;


#ifdef DEBUG
    if ( dbglev(MSPAC) == 3 )
      {
      fprintf(dbgfil(MSPAC),"***Start-wpdivs***\n");
      fprintf(dbgfil(MSPAC),"text=%s\n",text);
      fprintf(dbgfil(MSPAC),"maxpix=%d\n",maxpix);
      fflush(dbgfil(MSPAC));
      }
#endif

/*
***Beräkna antalet tecken som texten innehåller 
***och hur många pixels detta är.
*/
    sl = strlen(text);   
   *tdx = msstrl(text); 

#ifdef DEBUG
    if ( dbglev(MSPAC) == 3 )
      {
      fprintf(dbgfil(MSPAC),"sl=%d\n",sl);
      fprintf(dbgfil(MSPAC),"*tdx=%d\n",*tdx);
      fflush(dbgfil(MSPAC));
      }
#endif
/*
***textsträng <= max antal pixels som texten 
***får ta med hänsyn till aktuell skärmbredd.
*/
    if ( *tdx <= maxpix )    
      {
      strcpy(str1,text);
      strcpy(str2,"");
      *tdy = msstrh();
      }
/*
***textsträng > max antal pixels som texten 
***får ta med hänsyn till aktuell skärmbredd.
*/
    else                  
      {
/*
***Börja mitt i strängen, gå åt höger och leta upp första mellanslag.
*/
      slm = sl/2;

      while( slm < sl  &&  text[slm] != ' ') ++slm; 
/*
***Om det fanns ett delar vi strängen där ?
*/ 
      if ( slm < sl )
        {
        text[slm] = '\0';
        strcpy(str1,text);
        strcpy(str2,&text[slm+1]);
       *tdx = msstrl(str1);
       *tdy = 2*msstrh();
        }
/*
***Om inte börjar vi om i mitten och letar åt vänster istället.
*/
      else
        {
        slm = sl/2;
        while( slm > 0  &&  text[slm] != ' ') --slm; 

        if ( slm > 0 )
          {
          text[slm] = '\0';
          strcpy(str1,text);
          strcpy(str2,&text[slm+1]);
         *tdx = msstrl(str2);
         *tdy = 2*msstrh();
          }
/*
***Om inget mellanslag hittas där heller delar vi strängen mitt av.
*/
        else
          {
          slm = sl/2;
          text[slm] = '\0';
          strcpy(str1,text);
          strcpy(str2,&text[slm+1]);
         *tdx = msstrl(str1);
         *tdy = 2*msstrh();
          }
        }
      }

#ifdef DEBUG
    if ( dbglev(MSPAC) == 3 )
      {
      fprintf(dbgfil(MSPAC),"***Slut-wpdivs***\n\n");
      fflush(dbgfil(MSPAC));
      }
#endif

    return(0); 
  }

/********************************************************/
