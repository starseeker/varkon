/**********************************************************************
*
*    wpfont.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPfini();   Init font handling
*    WPgfnr();   Map font name -> number
*    WPgfnt();   Returns XFont pointer
*    WPsfnt();   Sets active font
*
*    WPstrl();   Length of string with active font
*    WPgtsl();   Length of string with specified font
*    WPstrh();   Height of active font
*    WPgtsh();   Height of any font
*    WPftpy();   Center text vertically
*    WPwstr();   Display text in window
*    WPdivs();   Split long line of text into two lines
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

#define FNTTBSIZ 20

typedef struct
{
char         name[V3STRLEN+1];
XFontStruct *xfs;
}WPFONT;


/*
***fnttab is a table of loaded fonts.
*/
static WPFONT fnttab[FNTTBSIZ];

/*
***actfnt is the number (index in fnttab) of the
***currently active font. actxfs is a C ptr to actfnt.
*/
static int          actfnt;
static XFontStruct *actxfs;

/********************************************************/

        short WPfini()

/*      Init fonts. Called by WPinit() at startup.
 *
 *      Felkod: WP1002 = Can't load font %s.
 *
 *      (C)2007-11-10 J.Kjellander
 *
 ******************************************************!*/

  {
    int          i;
    char         fntnam[V3STRLEN+1];
    XFontStruct *f;

/*
***Init fnttab[].
*/
    for ( i=0; i<FNTTBSIZ; ++i )
      {
      fnttab[i].name[0] = '\0';
      fnttab[i].xfs     = NULL;
      }
/*
***Load system text fonts.
*/
    if ( !WPgrst("varkon.font.small",fntnam) ) strcpy(fntnam,"fixed");
    if ( (f=XLoadQueryFont(xdisp,fntnam)) == NULL ) return(erpush("WP1022",fntnam));
    strcpy(fnttab[WP_FNTSMALL].name,fntnam);
    fnttab[WP_FNTSMALL].xfs = f;

    if ( !WPgrst("varkon.font.normal",fntnam) ) strcpy(fntnam,"fixed");
    if ( (f=XLoadQueryFont(xdisp,fntnam)) == NULL ) return(erpush("WP1022",fntnam));
    strcpy(fnttab[WP_FNTNORMAL].name,fntnam);
    fnttab[WP_FNTNORMAL].xfs = f;

    if ( !WPgrst("varkon.font.big",fntnam) ) strcpy(fntnam,"fixed");
    if ( (f=XLoadQueryFont(xdisp,fntnam)) == NULL ) return(erpush("WP1022",fntnam));
    strcpy(fnttab[WP_FNTBIG].name,fntnam);
    fnttab[WP_FNTBIG].xfs = f;
/*
***Activate normal font.
*/
    actfnt = -1;
    WPsfnt(WP_FNTNORMAL);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        int WPgfnr(char *fntnam)

/*      Loads a font into fnttab.
 *
 *      In: fntnam = Font name.
 *
 *      Return: Index to fnttab or -1.
 *
 *      Errors : WP1012 = fnttab full, font = %s.
 *               WP1022 = Font %s does not exist.
 *
 *      (C)2007-11-10J.Kjellander
 *
 ******************************************************!*/

  {
    int          i;
    XFontStruct *f;

/*
***Maybe this font is already loaded ?
*/
    for ( i=0; i<FNTTBSIZ; ++i )
      if ( strcmp(fnttab[i].name,fntnam) == 0  ) return(i);
/*
***No, find free entry in fnttab.
*/
    for ( i=0; i<FNTTBSIZ; ++i ) if ( fnttab[i].xfs == NULL ) break;
    if ( i == FNTTBSIZ ) return(erpush("WP1012",fntnam));
/*
***Try to load the font.
*/
    if ( (f=XLoadQueryFont(xdisp,fntnam)) == NULL )
      return(erpush("WP1022",fntnam));
/*
***All well, save in fnttb
*/
    strcpy(fnttab[i].name,fntnam);
    fnttab[i].xfs = f;
/*
***The end, return font number.
*/
    return(i);
  }

/********************************************************/
/*!******************************************************/

        XFontStruct *WPgfnt(int fntnum)

/*      Translates font number to  X Font structure ptr.
 *
 *      In: fntnum = Font number.
 *
 *      Return: C ptr to X Font structure.
 *
 *      (C)2007-11-10 J.Kjellander
 *
 *
 ******************************************************!*/

  {
    return(fnttab[fntnum].xfs);
  }

/********************************************************/
/*!******************************************************/

        void WPsfnt(int fntnum)

/*      Set active font.
 *
 *      In: fntnum = Font number.
 *
 *      (C)2007-11-10 J.Kjellander
 *
 *
 ******************************************************!*/

  {
/*
***If not already active, look up the font in
***fnttab and activate it.
*/
    if ( fntnum != actfnt )
      {
      actfnt = fntnum;
      XSetFont(xdisp,xgc,fnttab[fntnum].xfs->fid);
      actxfs = fnttab[fntnum].xfs;
      }
/*
***The end.
*/
    return;
  }

/********************************************************/
/*!******************************************************/

        int WPstrl(
        char *fstring)

/*      Return length of string in pixels. The currently
 *      active font is used.
 *
 *      In: fstring = String to measure.
 *
 *      Return: Length in pixels.
 *
 *      (C)2007-11-10 J.Kjellander
 *
 ******************************************************!*/

  {
    return(XTextWidth(actxfs,fstring,strlen(fstring)));
  }

/********************************************************/
/*!******************************************************/

        short WPgtsl(
        char *str,
        char *font,
        int  *plen)

/*      Return the length of a string with a named font.
 *
 *      In: str  = String to measure.
 *          font = Font name.
 *
 *      Out: *plen = Length in pixels.
 *
 *      (C)2007-11-10 J.Kjellander
 *
 ******************************************************!*/

  {
   short        fntnum;
   XFontStruct *fs;

/*
***Is it the currently active font ?
*/
   if ( *font == '\0' ) fs = actxfs;
/*
***If not, take it from fnttab. If it is not
***in fnttab it will automatically be loaded
***by WPgfnt().
*/
   else
     {
     if ( (fntnum=WPgfnr(font)) < 0 ) return(fntnum);
     else                             fs = WPgfnt(fntnum);
     }
/*
***Calculate the length.
*/
   *plen = XTextWidth(fs,str,strlen(str));
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        int WPstrh()

/*      Return the height of the currently active font.
 *
 *      FV: Height in pixels.
 *
 *      (C)2007-11-10 J.Kjellander
 *
 ******************************************************!*/

  {
    return(actxfs->ascent + actxfs->descent);
  }

/********************************************************/
/*!******************************************************/

        short WPgtsh(
        char *font,
        int  *phgt)

/*      Return the height of a named font.
 *
 *      In:   font = Font name.
 *
 *      Out: *phgt = Height in pixels.
 *
 *      (C)2007-11-10J.Kjellander
 *
 ******************************************************!*/

  {
   int          fntnum;
   XFontStruct *fs;
/*
***Maybe it's the currently active font.
*/
   if ( *font == '\0' ) fs = actxfs;
/*
***If not, take it from fnttab. If it is not
***in fnttab it will automatically be loaded
***by WPgfnt().
*/
   else
     {
     if ( (fntnum=WPgfnr(font)) < 0 ) return(fntnum);
     else                             fs = WPgfnt(fntnum);
     }
/*
***Calculate height.
*/
   *phgt = fs->ascent + fs->descent;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        int WPftpy(int dy)

/*      Calculates the Y coordinate for a text in pixels.
 *
 *      In: dy = Height of button/window etc.
 *
 *      return: Y-coordinate in pixels.
 *
 *      (C)2007-11-10 J.Kjellander
 *
 *
 ******************************************************!*/

  {
    return((dy - actxfs->descent + actxfs->ascent)/2);
  }

/********************************************************/
/********************************************************/

        short  WPwstr(
        Window wid,
        int    x,
        int    y,
        char  *s)

/*      Low level funtion for string output.
 *
 *      In: wid = X window id.
 *          x,y = Position relative to window.
 *          s   = C ptr to etring.
 *
 *      (C)2007-11-10 J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Draw string.
*/
   XDrawImageString(xdisp,wid,xgc,x,y,s,strlen(s));
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPdivs(
        char  text[],
        int   maxpix,
        int  *tdx,
        int  *tdy,
        char  str1[],
        char  str2[])

/*      Split a string in two parts.
 *
 *      In: text   = String to split.
 *          maxpix = Available space in pixels.
 *
 *      Out: tdx  = Size in X
 *           tdy  = Size in Y
 *           str1 = Part 1
 *           str2 = Part 2
 *
 *      (C)2007-11-10 J.Kjellander
 *
 ******************************************************!*/

  {
    int sl,slm;

/*
***Text length in charaters and pixels.
*/
    sl = strlen(text);
   *tdx = WPstrl(text);
/*
***Is splitting needed ?
*/
    if ( *tdx <= maxpix )
      {
      strcpy(str1,text);
      strcpy(str2,"");
     *tdy = WPstrh();
      }
/*
***Yes, split.
*/
    else
      {
/*
***Start in the middle of the string and search for a space.
*/
      slm = sl/2;

      while( slm < sl  &&  text[slm] != ' ') ++slm; 
/*
***If we found one, split.
*/
      if ( slm < sl )
        {
        text[slm] = '\0';
        strcpy(str1,text);
        strcpy(str2,&text[slm+1]);
       *tdx = WPstrl(str1);
       *tdy = 2*WPstrh();
        }
/*
***If not, start and search in the other direction (left).
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
         *tdx = WPstrl(str2);
         *tdy = 2*WPstrh();
          }
/*
***If no space is available, split the string in the middle.
*/
        else
          {
          slm = sl/2;
          text[slm] = '\0';
          strcpy(str1,text);
          strcpy(str2,&text[slm+1]);
         *tdx = WPstrl(str1);
         *tdy = 2*WPstrh();
          }
        }
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
