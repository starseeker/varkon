/**********************************************************************
*
*    wpcolor.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPcini();   Init color handling
*    WPccol();   Create color
*    WPcmat();   Create material
*    WPgcol();   Map pen number -> pixel value
*    WPgpen();   Returns RGB values
*    WPgmat();   Returns OpenGL Material values
*    WPspen();   Activate X11 pen
*    WPspenGL(); Activates OpenGL pen
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
#include <GL/glu.h>
#include <math.h>

/*
***The pen currently activated is actpen. actpen
***is used for for drawing in WPGWIN's (X11).
***actpen_gl is used for WPRWIN's (OpenGL).
***Valid pen values are in the range from 0 to
***WP_NPENS-1 + the WP_SPENS extra system colors
***used for window decoration etc. Pen 0 is the
***background color in all WPGWIN's and WPRWIN's.
*/
int actpen    = -1;
int actpen_gl = -1;

/*
***The WPPEN data structure holds the data needed to
***define the appearence of a Varkon pen. Pen numbers
***map to corresponding entries in pen_tab[]. A pen
***can either be a simple XColor RGB value or a full
***OpenGL material specification with ambient, diffuse,
***specular etc... Default XColors are created during
***system startup but no GL-materials. See the ini-file.
***MBS procedure CRE_COLOR() can create additional
***colors and CRE_MATERIAL() can create materials.
*/
typedef struct
{
int    red,green,blue;    /* Varkon/MBS color value */
XColor xrgb;              /* XColor value */
int    ambient[4];        /* Varkon/MBS Ambient RGB */
int    diffuse[4];        /* Varkon/MBS Diffuse RGB */
int    specular[4];       /* Varkon/MBS Specular RGB */
int    emission[4];       /* Varkon/MBS Emitted RGB */
int    shininess;         /* Varkon/MBS Shininess */
bool   defined;           /* Defined TRUE/FALSE */
bool   color;             /* Color or Material */
} WPPEN;


/*
***pentab is a table of WPPEN's, one pen for each entry.
***First WP_NPENS entry's are for user defined pens. Last
***WP_SPENS entry's are used for system specific colors.
*/
static WPPEN pen_tab[WP_NPENS+WP_SPENS];

/*
***Prototypes for internal functions.
*/
static void get_rgb(int pennum,XColor *xrgb);

/*!******************************************************/

        short WPcini()

/*      Init for color handling. Creates default colors
 *      based on the current ini-file. Called by
 *      WPinit() at startup.
 *
 *      (C)microform ab 13/12/93 J. Kjellander
 *
 *      1997-02-18 Omarbetad, J.Kjellander
 *      1999-03-24 Bug vid WPgrst, R. Svedin
 *      2007-03-08 1.19 J.Kjellander
 *      2008-01-17 Pens to jobdata, J.Kjellander
 *
 ******************************************************!*/

  {
    int       i;
    char      colnam[80],resurs[80];
    XColor    rgb,hardw;
    Colormap  colmap;

/*
***Start with an empty pentab[].
*/
   for ( i=0; i<WP_NPENS+WP_SPENS; ++i ) pen_tab[i].defined = FALSE;
/*
***Fix default colormap.
*/
    colmap = DefaultColormap(xdisp,xscr);
/*
***Pen 0 (background) defaults to white.
*/
    XLookupColor(xdisp,colmap,"White",&rgb,&hardw);
    XAllocColor(xdisp,colmap,&hardw);
    V3MOME(&hardw,&pen_tab[0].xrgb,sizeof(XColor));
    pen_tab[0].red     = (int)(255.0*(rgb.red/65535.0));
    pen_tab[0].green   = (int)(255.0*(rgb.green/65535.0));
    pen_tab[0].blue    = (int)(255.0*(rgb.blue/65535.0));
    pen_tab[0].defined = TRUE;
    pen_tab[0].color   = TRUE;
/*
***Pen 1 (foreground) defaults to black.
*/
    XLookupColor(xdisp,colmap,"Black",&rgb,&hardw);
    XAllocColor(xdisp,colmap,&hardw);
    V3MOME(&hardw,&pen_tab[1].xrgb,sizeof(XColor));
    pen_tab[1].red     = (int)(255.0*(rgb.red/65535.0));
    pen_tab[1].green   = (int)(255.0*(rgb.green/65535.0));
    pen_tab[1].blue    = (int)(255.0*(rgb.blue/65535.0));
    pen_tab[1].defined = TRUE;
    pen_tab[1].color   = TRUE;
/*
***The 6 system colors.
**
***Background color in WPIWIN's.
*/
    if ( !WPgrst("varkon.background",colnam) ) strcpy(colnam,"LightGray");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) )
      {
      V3MOME(&hardw,&pen_tab[WP_BGND1].xrgb,sizeof(XColor));
      pen_tab[WP_BGND1].red     = (int)(255.0*(rgb.red/65535.0));
      pen_tab[WP_BGND1].green   = (int)(255.0*(rgb.green/65535.0));
      pen_tab[WP_BGND1].blue    = (int)(255.0*(rgb.blue/65535.0));
      pen_tab[WP_BGND1].defined = TRUE;
      pen_tab[WP_BGND1].color   = TRUE;
      }
/*
***Background color in normal buttons.
*/
    if ( !WPgrst("varkon.buttonColor",colnam) ) strcpy(colnam,"Gray");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) )
      {
      V3MOME(&hardw,&pen_tab[WP_BGND2].xrgb,sizeof(XColor));
      pen_tab[WP_BGND2].red     = (int)(255.0*(rgb.red/65535.0));
      pen_tab[WP_BGND2].green   = (int)(255.0*(rgb.green/65535.0));
      pen_tab[WP_BGND2].blue    = (int)(255.0*(rgb.blue/65535.0));
      pen_tab[WP_BGND2].defined = TRUE;
      pen_tab[WP_BGND2].color   = TRUE;
      }
/*
***Background color in "selected" buttons.
*/
    if ( !WPgrst("varkon.selectedButtonColor",colnam) ) strcpy(colnam,"Gray");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) )
      {
      V3MOME(&hardw,&pen_tab[WP_BGND3].xrgb,sizeof(XColor));
      pen_tab[WP_BGND3].red     = (int)(255.0*(rgb.red/65535.0));
      pen_tab[WP_BGND3].green   = (int)(255.0*(rgb.green/65535.0));
      pen_tab[WP_BGND3].blue    = (int)(255.0*(rgb.blue/65535.0));
      pen_tab[WP_BGND3].defined = TRUE;
      pen_tab[WP_BGND3].color   = TRUE;
      }
/*
***Foreground color, ie. for text etc.
*/
    if ( !WPgrst("varkon.foreground",colnam) ) strcpy(colnam,"Black");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) )
      {
      V3MOME(&hardw,&pen_tab[WP_FGND].xrgb,sizeof(XColor));
      pen_tab[WP_FGND].red     = (int)(255.0*(rgb.red/65535.0));
      pen_tab[WP_FGND].green   = (int)(255.0*(rgb.green/65535.0));
      pen_tab[WP_FGND].blue    = (int)(255.0*(rgb.blue/65535.0));
      pen_tab[WP_FGND].defined = TRUE;
      pen_tab[WP_FGND].color   = TRUE;
      }
/*
***Top shadow for icons/buttons. Bottom for edits.
*/
    if ( !WPgrst("varkon.topShadowColor",colnam) ) strcpy(colnam,"White");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) )
      {
      V3MOME(&hardw,&pen_tab[WP_TOPS].xrgb,sizeof(XColor));
      pen_tab[WP_TOPS].red     = (int)(255.0*(rgb.red/65535.0));
      pen_tab[WP_TOPS].green   = (int)(255.0*(rgb.green/65535.0));
      pen_tab[WP_TOPS].blue    = (int)(255.0*(rgb.blue/65535.0));
      pen_tab[WP_TOPS].defined = TRUE;
      pen_tab[WP_TOPS].color   = TRUE;
      }
/*
***Bottom shadow for icons/buttons. Top for edits.
*/
    if ( !WPgrst("varkon.bottomShadowColor",colnam) ) strcpy(colnam,"Black");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) )
      {
      V3MOME(&hardw,&pen_tab[WP_BOTS].xrgb,sizeof(XColor));
      pen_tab[WP_BOTS].red     = (int)(255.0*(rgb.red/65535.0));
      pen_tab[WP_BOTS].green   = (int)(255.0*(rgb.green/65535.0));
      pen_tab[WP_BOTS].blue    = (int)(255.0*(rgb.blue/65535.0));
      pen_tab[WP_BOTS].defined = TRUE;
      pen_tab[WP_BOTS].color   = TRUE;
      }
/*
***Notify color for button/icon crossing events.
*/
    if ( !WPgrst("varkon.highlightedButtonColor",colnam) ) strcpy(colnam,"Blue");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) )
      {
      V3MOME(&hardw,&pen_tab[WP_NOTI].xrgb,sizeof(XColor));
      pen_tab[WP_NOTI].red     = (int)(255.0*(rgb.red/65535.0));
      pen_tab[WP_NOTI].green   = (int)(255.0*(rgb.green/65535.0));
      pen_tab[WP_NOTI].blue    = (int)(255.0*(rgb.blue/65535.0));
      pen_tab[WP_NOTI].defined = TRUE;
      pen_tab[WP_NOTI].color   = TRUE;
      }
/*
***Background color in tooltip windows.
*/
    if ( !WPgrst("varkon.tooltipColor",colnam) ) strcpy(colnam,"White");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) )
      {
      V3MOME(&hardw,&pen_tab[WP_TTIP].xrgb,sizeof(XColor));
      pen_tab[WP_TTIP].red     = (int)(255.0*(rgb.red/65535.0));
      pen_tab[WP_TTIP].green   = (int)(255.0*(rgb.green/65535.0));
      pen_tab[WP_TTIP].blue    = (int)(255.0*(rgb.blue/65535.0));
      pen_tab[WP_TTIP].defined = TRUE;
      pen_tab[WP_TTIP].color   = TRUE;
      }
/*
***Entity highlight.
*/
    if ( !WPgrst("varkon.highlightedEntityColor",colnam) ) strcpy(colnam,"Orange");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) )
      {
      V3MOME(&hardw,&pen_tab[WP_ENTHG].xrgb,sizeof(XColor));
      pen_tab[WP_ENTHG].red     = (int)(255.0*(rgb.red/65535.0));
      pen_tab[WP_ENTHG].green   = (int)(255.0*(rgb.green/65535.0));
      pen_tab[WP_ENTHG].blue    = (int)(255.0*(rgb.blue/65535.0));
      pen_tab[WP_ENTHG].defined = TRUE;
      pen_tab[WP_ENTHG].color   = TRUE;
      }
/*
***And then finally the pen colors from the ini-file.
*/
    for ( i=0; i<WP_NPENS; ++i )
      {
      sprintf(resurs,"varkon.color_%d",i);
      if ( WPgrst(resurs,colnam) )
        {
        XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
        if ( XAllocColor(xdisp,colmap,&hardw) )
          {
          V3MOME(&hardw,&pen_tab[i].xrgb,sizeof(XColor));
          pen_tab[i].red     = (int)(255.0*(rgb.red/65535.0));
          pen_tab[i].green   = (int)(255.0*(rgb.green/65535.0));
          pen_tab[i].blue    = (int)(255.0*(rgb.blue/65535.0));
          pen_tab[i].defined = TRUE;
          pen_tab[i].color   = TRUE;
          }
        }
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/********************************************************/

        short WPccol(
        int   pen,
        int   red,
        int   green,
        int   blue)

/*      Allocates an X color defined by the RGB value
 *      supplied. Used during startup and by CRE_COLOR()
 *      in MBS.
 *
 *      In: pen            = Pen number.
 *          red,green,blue = MBS RGB-values 0 -> 255.
 *
 *      (C)microform ab 1997-02-18 J. Kjellander
 *
 *      2007-01-31, 1.19 J.Kjellander
 *
 ******************************************************!*/

  {
    XColor    rgb;
    Colormap  colmap;
    double    tmp;

/*
***Fix default colormap.
*/
    colmap = DefaultColormap(xdisp,xscr);
/*
***Input check.
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
***Init an XColor. X RGB values are scaled to 2**16.
***Varkon/MBS RGB values are scaled 2**8.
*/
   tmp = (double)red*65535.0/255.0;
   rgb.red   = (unsigned short)tmp;

   tmp = (double)green*65535.0/255.0;
   rgb.green = (unsigned short)tmp;

   tmp = (double)blue*65535.0/255.0;
   rgb.blue  = (unsigned short)tmp;
/*
***Allocate the color with X and save in pen_tab.
***Also save original MBS RGB values. The allocated
***color may differ slightly depending on hardware.
*/
   if ( XAllocColor(xdisp,colmap,&rgb) )
     {
     V3MOME(&rgb,&pen_tab[pen].xrgb,sizeof(XColor));
     pen_tab[pen].red     = red;
     pen_tab[pen].green   = green;
     pen_tab[pen].blue    = blue;
     pen_tab[pen].defined = TRUE;
     pen_tab[pen].color   = TRUE;
     }
/*
***If this pen is the current pen, it needs to be reloaded
***so that subsequent draws will reflect the change in color.
*/
   if ( pen == actpen ) actpen = -1;
   if ( pen == actpen_gl ) actpen_gl = -1;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

        short WPcmat(
        int   pen,
        int   ar,
        int   ag,
        int   ab,
        int   dr,
        int   dg,
        int   db,
        int   sr,
        int   sg,
        int   sb,
        int   er,
        int   eg,
        int   eb,
        int   sh)

/*      Create a GL material.
 *
 *      In: pen      = Pen number
 *          ar,ag,ab = Ambient RGB   0-255
 *          dr,dg,db = Diffuse RGB   0-255
 *          sr,sg,sb = Specular RGB  0-255
 *          er,eg,eb = Emitted RGB   0-255
 *          sh       = Shininess     0-255
 *
 *      (C)microform ab 1997-02-20 J. Kjellander
 *
 *      2007-02-02 1,19 J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Error checks.
*/
   if ( pen < 1  ||  pen > WP_NPENS-1 ) return(0);

   if ( ar < 0  ||  ar > 255 ) return(0);
   if ( ag < 0  ||  ag > 255 ) return(0);
   if ( ab < 0  ||  ab > 255 ) return(0);
   if ( dr < 0  ||  dr > 255 ) return(0);
   if ( dg < 0  ||  dg > 255 ) return(0);
   if ( db < 0  ||  db > 255 ) return(0);
   if ( sr < 0  ||  sr > 255 ) return(0);
   if ( sg < 0  ||  sg > 255 ) return(0);
   if ( sb < 0  ||  sb > 255 ) return(0);
   if ( er < 0  ||  er > 255 ) return(0);
   if ( eg < 0  ||  eg > 255 ) return(0);
   if ( eb < 0  ||  eb > 255 ) return(0);
   if ( sh < 0  ||  sh > 255 ) return(0);
/*
***Save material specification in pen_tab[].
*/
   pen_tab[pen].ambient[0]  = ar;
   pen_tab[pen].ambient[1]  = ag;
   pen_tab[pen].ambient[2]  = ab;
   pen_tab[pen].ambient[3]  = 255;
   pen_tab[pen].diffuse[0]  = dr;
   pen_tab[pen].diffuse[1]  = dg;
   pen_tab[pen].diffuse[2]  = db;
   pen_tab[pen].diffuse[3]  = 255;
   pen_tab[pen].specular[0] = sr;
   pen_tab[pen].specular[1] = sg;
   pen_tab[pen].specular[2] = sb;
   pen_tab[pen].specular[3] = 255;
   pen_tab[pen].emission[0] = er;
   pen_tab[pen].emission[1] = eg;
   pen_tab[pen].emission[2] = eb;
   pen_tab[pen].emission[3] = 255;
   pen_tab[pen].shininess   = sh;
/*
***Also save the ambient color as the overall color.
***This makes it possible to visualize in WPGWIN and
***at least get a color.
*/
   WPccol(pen,ar,ag,ab);
/*
***This pen is defined as a material.
*/
   pen_tab[pen].defined = TRUE;
   pen_tab[pen].color   = FALSE;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        unsigned long WPgcol(
        int pennum)

/*      Returns the X rgb.pixel value of a pen in pen_tab[].
 *
 *      In: pennum = Pen number.
 *
 *      Return: Pixel value.
 *
 *      (C)microform ab 12/12/93 J. Kjellander
 *
 *      1997-02-18 Omarbetad, J.Kjellander
 *      2007-02-02 1.19 J.Kjellander
 *
 ******************************************************!*/

  {
    if ( pennum < 0 ) pennum = 0;
    if ( pennum > WP_NPENS+WP_SPENS-1 ) pennum = WP_NPENS+WP_SPENS-1;

    return(pen_tab[pennum].xrgb.pixel);
  }

/********************************************************/
/********************************************************/

        short WPgpen(
        int   pen,
        bool *defined,
        int  *red,
        int  *green,
        int  *blue)

/*      Returns the RGB values of a pen in pen_tab[].
 *      Note that these values may differ from what was
 *      specified by the user when the color was created
 *      because X allocates the closest color. Used by
 *      GET_COLOR() in MBS.
 *
 *      In: pen = Pen number.
 *
 *      Out: *defined = TRUE if defined, FALSE if not.
 *           *red     = Red value 0-255
 *           *green   = Green value 0-255
 *           *blue    = Blue value 0-255
 *
 *      Return:     0 => Ok.
 *             WP1682 => Invalid pen number
 *
 *      (C)2008-01.19 J.Kjellander
 *
 ******************************************************!*/

  {
    char errbuf[V3STRLEN];
/*
***Check that the pen number is valid.
*/
    if ( pen < 0  ||  pen > WP_NPENS+WP_SPENS-1 )
      {
      sprintf(errbuf,"%d",pen);
      return(erpush("WP1682",errbuf));
      }
/*
***Is this pen defined ?
*/
   if ( (*defined=pen_tab[pen].defined) )
     {
/*
***Yes, map rgb values to MBS.
*/
     *red   = pen_tab[pen].red;
     *green = pen_tab[pen].green;
     *blue  = pen_tab[pen].blue;
     }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/********************************************************/

        short WPgmat(
        int   pen,
        bool *defined,
        int  *ar,
        int  *ag,
        int  *ab,
        int  *dr,
        int  *dg,
        int  *db,
        int  *sr,
        int  *sg,
        int  *sb,
        int  *er,
        int  *eg,
        int  *eb,
        int  *s)

/*      Returns the material values of a pen in pen_tab[].
 *      GET_MATERIAL() in MBS.
 *
 *      In: pen = Pen number.
 *
 *      Out: *defined = TRUE if material is defined, FALSE if not.
 *
 *      Return:     0 => Ok.
 *             WP1682 => Invalid pen number
 *
 *      (C)2008-01.19 J.Kjellander
 *
 ******************************************************!*/

  {
   char errbuf[V3STRLEN];
/*
***Check that the pen number is valid.
*/
   if ( pen < 0  ||  pen > WP_NPENS+WP_SPENS-1 )
     {
     sprintf(errbuf,"%d",pen);
     return(erpush("WP1682",errbuf));
     }
/*
***Is this pen defined as a material ?
*/
   if ( pen_tab[pen].defined && !pen_tab[pen].color )
     {
/*
***Yes.
*/
     *defined = TRUE;
     *ar = pen_tab[pen].ambient[0];
     *ag = pen_tab[pen].ambient[1];
     *ab = pen_tab[pen].ambient[2];
     *dr = pen_tab[pen].diffuse[0];
     *dg = pen_tab[pen].diffuse[1];
     *db = pen_tab[pen].diffuse[2];
     *sr = pen_tab[pen].specular[0];
     *sg = pen_tab[pen].specular[1];
     *sb = pen_tab[pen].specular[2];
     *er = pen_tab[pen].emission[0];
     *eg = pen_tab[pen].emission[1];
     *eb = pen_tab[pen].emission[2];
     *s  = pen_tab[pen].shininess;
     }
/*
***No.
*/
   else *defined = FALSE;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

        short WPspen(
        int   pen)

/*      Activates the specified pen (color) in a WPGWIN.
 *
 *      In: pen = Pen number.
 *
 *      (C)microform ab 31/12/94 J. Kjellander
 *
 *      1997-12-26 Separata GC, J.Kjellander
 *      2007-02-02 1.19 J.Kjellander
 *
 ********************************************************/

{
   int     i;
   WPGWIN *gwinpt;

/*
***Loop through all WPGWIN and set the foreground color in their GC's/DC's.
*/
#ifdef UNIX
   XFlush(xdisp);
#endif
   if ( pen != actpen )
     {
     for ( i=0; i<WTABSIZ; ++i)
       {
       if ( wpwtab[i].ptr != NULL )
         {
         if ( wpwtab[i].typ == TYP_GWIN )
           {
           gwinpt = (WPGWIN *)wpwtab[i].ptr;
#ifdef WIN32
           SelectObject(gwinpt->dc,msgcol(pen));
           SelectObject(gwinpt->bmdc,msgcol(pen));
#endif
#ifdef UNIX
           XSetForeground(xdisp,gwinpt->win_gc,WPgcol(pen));
#endif
           }
         }
       }
/*
***Update actpen.
*/
     actpen = pen;
     }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short   WPspenGL(
        WPRWIN *rwinpt,
        int     pen)

/*      Activates the color or material associated with
 *      a pen for display with OpenGL (WPRWIN).
 *
 *      In: rwinpt = C-ptr to OpenGL window
 *          pen    = Pen number.
 *
 *      (C)microform ab 1997-02-11 J. Kjellander
 *
 *      1997-04-13 WIN32, J.Kjellander
 *      1997-06-11 rgb_gl, J.Kjellander
 *      2007-02-02 1.19 J.Kjellander
 *
 ******************************************************!*/

  {
   GLfloat mat[4];
   XColor  rgb;
   GLint   cind[3];

/*
***Is the pen defined ?
*/
   if ( pen > 0  &&  pen < WP_NPENS+WP_SPENS  &&  pen_tab[pen].defined )
     {
/*
***If it is defined as a color, use the color.
*/
     if ( pen_tab[pen].color )
       {
       if ( rwinpt->rgb_mode )
         {
         get_rgb(pen,&rgb);
         mat[0] = rgb.red/65536.0;
         mat[1] = rgb.green/65536.0;
         mat[2] = rgb.blue/65536.0;
         mat[3] = 1.0;

         glMaterialfv(GL_FRONT,GL_AMBIENT,mat);
         glMaterialfv(GL_FRONT,GL_DIFFUSE,mat);
         glMaterialfv(GL_FRONT,GL_SPECULAR,mat);
         mat[0] = mat[1] = mat[2] = 0.0;
         glMaterialfv(GL_FRONT,GL_EMISSION,mat);
         glMaterialf(GL_FRONT,GL_SHININESS,(GLfloat)40.0);
         }
       else
         {
         cind[0] = cind[1] = cind[2] = (GLint)pen;
         glMaterialiv(GL_FRONT,GL_COLOR_INDEXES,cind);
         }
       }
/*
***Else, use the material.
*/
     else
       {
       mat[0] = (GLfloat)(pen_tab[pen].ambient[0])/(GLfloat)255.0;
       mat[1] = (GLfloat)(pen_tab[pen].ambient[1])/(GLfloat)255.0;
       mat[2] = (GLfloat)(pen_tab[pen].ambient[2])/(GLfloat)255.0;
       mat[3] = (GLfloat)(pen_tab[pen].ambient[3])/(GLfloat)255.0;
       glMaterialfv(GL_FRONT,GL_AMBIENT,mat);

       mat[0] = (GLfloat)(pen_tab[pen].diffuse[0])/(GLfloat)255.0;
       mat[1] = (GLfloat)(pen_tab[pen].diffuse[1])/(GLfloat)255.0;
       mat[2] = (GLfloat)(pen_tab[pen].diffuse[2])/(GLfloat)255.0;
       mat[3] = (GLfloat)(pen_tab[pen].diffuse[3])/(GLfloat)255.0;
       glMaterialfv(GL_FRONT,GL_DIFFUSE,mat);

       mat[0] = (GLfloat)(pen_tab[pen].specular[0])/(GLfloat)255.0;
       mat[1] = (GLfloat)(pen_tab[pen].specular[1])/(GLfloat)255.0;
       mat[2] = (GLfloat)(pen_tab[pen].specular[2])/(GLfloat)255.0;
       mat[3] = (GLfloat)(pen_tab[pen].specular[3])/(GLfloat)255.0;
       glMaterialfv(GL_FRONT,GL_SPECULAR,mat);

       mat[0] = (GLfloat)(pen_tab[pen].emission[0])/(GLfloat)255.0;
       mat[1] = (GLfloat)(pen_tab[pen].emission[1])/(GLfloat)255.0;
       mat[2] = (GLfloat)(pen_tab[pen].emission[2])/(GLfloat)255.0;
       mat[3] = (GLfloat)(pen_tab[pen].emission[3])/(GLfloat)255.0;
       glMaterialfv(GL_FRONT,GL_EMISSION,mat);

       glMaterialf(GL_FRONT,GL_SHININESS,(GLfloat)pen_tab[pen].shininess/(GLfloat)2.0);
       }
     }
/*
***Update actpen_gl.
*/
   actpen_gl = pen;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static void    get_rgb(
        int     pennum,
        XColor *xrgb)

/*      Returns the X color structure of a specific
 *      pen in pen_tab[].
 *
 *      In:  pennum = Pen number.
 *
 *      Out: *rgb   = XColor-structure.
 *
 *      (C)microform ab 1997-02-17 J. Kjellander
 *
 *      2007-02-02 1.19 J.Kjellander
 *
 ******************************************************!*/

  {
    if ( pennum < 0 ) pennum = 0;
    if ( pennum > WP_NPENS+WP_SPENS-1 ) pennum = WP_NPENS+WP_SPENS-1;

    V3MOME(&pen_tab[pennum].xrgb,xrgb,sizeof(XColor));
  }

/********************************************************/
