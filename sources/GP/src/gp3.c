/**********************************************************************
*
*    gp3.c
*    =====
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gp4010();    Switch to Tek-4010 mode
*    gpansi();    Switch to VT100 mode
*    gpflsh();    Flush output buffer
*    gpersc();    Erase screen
*    gperdf();    Erase display file
*    gperal();    Erase screen and display file
*    gpdram();    Draw a frame
*    gpmvsc();    Move graphic pen
*    gpdwsc();    Draw vector
*    gpgtmc();    Returns model coordinate
*    gpgtsc();    Returns screen coordinate
*    gpdpmk();    Draw pointer marker
*    gpepmk();    Erase all pointer markers
*    gpdmrk();    Draw marker
*    gpspen();    Set active pen
*    gpsbrush();  Set active brush
*    gpswdt();    Set active linewidth
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
#include "../../WP/include/WP.h"
#include "../include/GP.h"

#ifdef UNIX
#include <unistd.h>
#endif

static char vstr[17];
static char oldhiy,oldloy,oldhix;

/* vstr är den sträng som används för att skicka
   koordinater till en grafisk skärm. old-variablerna
   används för att kolla om kort adressering är möjlig
   när terminalen är av typ Tek-4010. */


#ifdef V3_X11
extern XPoint   xbuf[];
extern int      xncrd;
extern Display *xdisp;
extern Window   xgwin; 
extern GC       gpxgc;
extern Pixmap   xgmap;
extern int      xscr;

#endif

#ifdef WIN32
extern POINT    ms_buf[];
extern int      ms_ncrd;
extern HDC      ms_dc,ms_bmdc;
#endif

extern short  tmode;
extern short  gptrty;
extern short  pmkx[];
extern short  pmky[];
extern short  npmk;
extern short  actpen;
extern gmint  dfpek,dfcur;
extern double viewpt[];
extern VY     actvy;
extern short  gpsnpx,gpsnpy,gpgnpx,gpgnpy,gpgorx,gpgory;
extern char   to4010[],toansi[],pen0[],pen1[];
extern short  rmarg,bmarg,mhgt;

/********************************************************/

        short gp4010()

/*      Ställer om terminal till Tektronix 4010-mode.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 31/10/84 J. Kjellander
 * 
 *      3/2/86   Flagga för terminalmode R. Svedin
 *      16/1/87  ALFA terminal, R. Svedin
 *
 ********************************************************/

{
   if ( gptrty != ALFA )
     {
     if ( tmode != 1 )
       {
       fputs(to4010,stdout);
       tmode = 1;
       oldhiy = oldloy = oldhix = '\0';
       }
     }

   return(0);
}

/********************************************************/
/********************************************************/

        short gpansi()

/*      Ställer om terminal till VT100-mode.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 31/10/84 J. Kjellander
 *
 *      3/2/86   Flagga för terminalmode R. Svedin
 *      16/1/87  ALAFA terminal, R. Svedin
 *
 ********************************************************/

{
   if ( gptrty != ALFA )
     {
     if ( tmode != 0 )
       {
       fputs(toansi,stdout);
       tmode = 0;
       }
     }

   return(0);
}

/********************************************************/
/********************************************************/

        short gpflsh()

/*      Tömmer Grapacs-outputbuffer.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 16/10/88 J. Kjellander
 *
 *      16/6/92  X-Windows, J. Kjellander
 *      3/11/95  WIN32, J. Kjellander
 *
 ********************************************************/

{
#ifdef V3_X11
   if ( xncrd > 0 )
     {
     XDrawLines(xdisp,xgwin,gpxgc,xbuf,xncrd,CoordModeOrigin);
     XDrawLines(xdisp,xgmap,gpxgc,xbuf,xncrd,CoordModeOrigin);
     XFlush(xdisp);
     xncrd = 0;
     }
#endif

#ifdef WIN32
   if ( ms_ncrd > 1 )
     {
     Polyline(ms_dc,ms_buf,ms_ncrd);
     Polyline(ms_bmdc,ms_buf,ms_ncrd);
     ms_ncrd = 0;
     }
#endif

   return(0);
}

/********************************************************/
/********************************************************/

        short gpersc()

/*      Suddar grafisk skärm. Nollställer pekmärkes-
 *      räknaren.
 *
 *      (C)microform ab 31/10/84 J. Kjellander
 *
 *      12/1/86  IP_32, J. Kjellander
 *      3/2/86   Flagga för terminalmode R. Svedin
 *      16/5/86  Vänta en sekund (sleep) R. Svedin
 *      7/1/87   Nya terminaler FT4600, ALFA, R. Svedin
 *      7/12/87  Ny terminal MG700, B. Doverud
 *      21/1/88  Nya terminaler T4207, MO2000, B. Doverud
 *      31/5/88  Ny terminal TDV25, B. Doverud
 *      16/10/88 CGI, J. Kjellander
 *      20/10/88 N220G ersätter TDV25, B. Doverud
 *      16/6/92  X-Windows, J. Kjellander
 *
 ********************************************************/

{

  switch ( gptrty )
    {
/*
***Luxor VT100.
*/
    case LVT100:
    igmvac(1,mhgt+2);
    igerar(rmarg,bmarg-(mhgt+1));
    break;
/*
***Tektronix 4010.
*/
    case V550:
    case MG400:
    case MG420:
    case FT4600:
    case MG700:
    case T4207:
    case N220G:
    case MX7250:
    case MSCOLOUR:
    case MSMONO:
    gp4010();
    fputs("\033\014",stdout);
#ifndef WIN32
    sleep(1);
#endif
    break;
/*
***Modgraf.
*/
    case MO2000:
    gp4010();
    fputs("\033\031",stdout);
#ifndef WIN32
    sleep(1);
#endif
    break;
/*
***X11. Sudda v3:s huvudfönster.
*/
#ifdef V3_X11
    case X11:
    wpergw(GWIN_MAIN);
    break;
#endif
/*
***Microsoft Windows.
*/
#ifdef WIN32
    case MSWIN:
    msergw(GWIN_MAIN);
    break;
#endif
    }
/*
***Efter att skärmen suddats är inga pekmärken synliga.
*/
    npmk = 0;

    return(0);
}

/********************************************************/
/*!******************************************************/

        short gperal()

/*      Suddar skärm och tömmer df.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 4/12-85 J. Kjellander
 *
 ******************************************************!*/

  {
    gpersc();
    gperdf();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gperdf()

/*      Suddar df.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 2/1-95 J. Kjellander
 *
 ******************************************************!*/

  {
    dfpek = -1;
    dfcur =  dfpek;

    return(0);
  }

/********************************************************/
/********************************************************/

        short gpdram()

/*      Ritar en ram på skärmen.
 *
 *      (C)microform ab 31/10/84 J. Kjellander
 *
 *      15/6/86  Case IP3215 R. Svedin
 *      7/1/87   Nya terminaler FT4600, ALFA, R. Svedin
 *      7/12/87  Ny terminal MG700 B. Doverud
 *      21/1/88  Nya terminaler T4207, MO2000, B.Doverud
 *      31/5/88  Ny terminal TDV25, B. Doverud
 *      16/10/88 CGI, J. Kjellander
 *      20/10/88 N220G ersätter TDV25, B. Doverud 
 *
 ********************************************************/

{

    gpspen(1);

    switch (gptrty)
      {
      case LVT100:
      gpmvsc(gpgorx,gpgory);
      gpdwsc(gpgnpx,gpgory);
      gpdwsc(gpgnpx,1015);
      gpdwsc(gpgorx,1015);
      gpdwsc(gpgorx,gpgory);
      gpmvsc(gpgorx-2,gpgory-2);
      gpdwsc(gpgnpx+2,gpgory-2);
      gpdwsc(gpgnpx+2,1017);
      gpdwsc(gpgorx-2,1017);
      gpdwsc(gpgorx-2,gpgory-2);
      gpmvsc(gpgorx,gpgnpy);
      gpdwsc(gpgnpx,gpgnpy);
      break;

      case V550:
      case MG400:
      case MG420:
      case FT4600:
      case MG700:
      case T4207:
      case MO2000:
      case MX7250:
      gpmvsc(gpgorx,gpgory);
      gpdwsc(gpgnpx,gpgory);
      gpdwsc(gpgnpx,gpgnpy);
      gpdwsc(gpgorx,gpgnpy);
      gpdwsc(gpgorx,gpgory);
      break;

      case N220G:
      gpmvsc(1,1);
      gpdwsc(gpsnpx-1,1);
      gpdwsc(gpsnpx-1,gpsnpy-1);
      gpdwsc(1,gpsnpy-1);
      gpdwsc(1,1);
      case MSCOLOUR:
      gpspen(4);
      case MSMONO:
      gpmvsc(gpsnpx-1,gpgory-1);
      gpdwsc(gpgorx,gpgory-1);
      gpdwsc(gpgorx,gpsnpy-1);
      gpspen(1);
      break;
      }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short gpmvsc(
        short ix,
        short iy)

/*      Move pen on screen. x,y är en skärmkoordinat på
 *      heltalsformat.
 *
 *      (C)microform ab 31/10/84 J. Kjellander
 *
 *      12/1/86  IP_32, J. Kjellander
 *      3/2/86   Flagga för terminalmode R. Svedin
 *      14/2/86  Olika terminaltyper, J. Kjellander
 *      19/3/86  gpitoa(), J. Kjellander
 *      7/1/87   Nya terminaler FT4600, ALFA, R. Svedin
 *      7/12/87  Ny terminal MG700 B. Doverud
 *      21/1/88  Nya terminaler T4207, MO2000, B. Doverud
 *      31/5/88  Ny terminal TDV25 B. Doverud
 *      16/10/88 CGI, J. Kjellander
 *      20/10/88 N220G ersätter TDV25, B. Doverud 
 *      16/6/92  X-Windows, J. Kjellander
 *
 ******************************************************!*/

{
    register char *pvstr;

    switch ( gptrty )
      {
/*
***Luxor VT100.
*/
      case LVT100:
      pvstr = vstr;
      *pvstr++ = 27;
      *pvstr++ = ':';
      pvstr = gpitoa(pvstr,ix);
      *pvstr++ = ';';
      pvstr = gpitoa(pvstr,iy);
      *pvstr++ = 'm';
      *pvstr = '\0';
      fputs(vstr,stdout);
      break;

/*
***Tektronix 4010.
*/
      case V550:
      case MG400:
      case MG420:
      case FT4600:
      case MG700:
      case T4207:
      case MO2000:
      case N220G:
      case MX7250:
      case MSCOLOUR:
      case MSMONO:
      gp4010();
      putchar(29);
      gpdwsc(ix,iy);
      break;
/*
***X-Windows.
*/
#ifdef V3_X11
      case X11:
      if ( xncrd > 1 ) gpflsh();
      xbuf[0].x = ix;
      xbuf[0].y = gpsnpy-iy;
      xncrd = 1;
      break;
#endif
/*
***Microsoft Windows.
*/
#ifdef WIN32
      case MSWIN:
      if ( ms_ncrd > 1 ) gpflsh();
      ms_buf[0].x = ix;
      ms_buf[0].y = gpsnpy-iy;
      ms_ncrd = 1;
      break;
#endif
      }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short gpdwsc(
        short ix,
        short iy)

/*      Draw on screen. x,y är en skärmkoordinat på
 *      heltalsformat. Koordinaten bör ligga inom gräns-
 *      erna för skärmens adresseringsområde.
 *
 *      (C)microform ab 31/10/84 J. Kjellander
 *
 *      12/1/86  IP_32, J. Kjellander
 *      3/2/86   Flagga för terminalmode R. Svedin
 *      17/2/86  Kort adressering, J. Kjellander
 *      19/3/86  gpitoa(), J. Kjellander
 *      14/11/86 Bug tektronix, J. Kjellander
 *      7/1/87   Nya terminaler FT4600, ALFA, R. Svedin
 *      7/12/87  Ny terminal MG700 B. Doverud
 *      21/1/88  Nya terminaler T4207, MO2000, B. Doverud
 *      31/5/88  Ny terminal TDV25, B. Doverud
 *      16/10/88 CGI, J. Kjellander
 *      20/10/88 N220G ersätter TDV25, B. Doverud 
 *      16/6/92  X-Windows, J. Kjellander
 *
 ******************************************************!*/

{
    register char *pvstr = vstr;
    char newhiy,newloy,newhix,newlox;
    register short tmp;

    switch ( gptrty )
      {
/*
***Luxor VT100.
*/
      case LVT100:
      *pvstr++ = 27;
      *pvstr++ = ':';
      pvstr = gpitoa(pvstr,ix);
      *pvstr++ = ';';
      pvstr = gpitoa(pvstr,iy);
      *pvstr++ = ';';
      *pvstr++ = '0';
      *pvstr++ = ';';
      if ( actpen == 0 ) *pvstr++ = '0';
      else               *pvstr++ = '1';
      *pvstr++ = 'd';
      *pvstr = '\0';
      fputs(vstr,stdout);
      break;
/*
***Om FACIT vrid koordinatsystemet 90 grader.
*/
      case FT4600:                      
      tmp = ix;
      ix = gpgnpy - iy;
      iy = tmp;
/*
***Tektronix 4010.
*/
      case V550:
      case MG400:
      case MG420:
      case MG700:
      case T4207:
      case MO2000:
      case N220G:
      case MX7250:
      case MSCOLOUR:
      case MSMONO:
      newhiy = ((iy & 992)>>5)|32;      /* High byte Y 01xxxxx */
      newloy = (iy&31)|96;              /* Low byte Y  11xxxxx */
      newhix = ((ix & 992)>>5)|32;      /* High byte X 01xxxxx */
      newlox = (ix&31)|64;              /* Low byte X  10xxxxx */

      if ( newhiy != oldhiy )
        {
        *pvstr = oldhiy = newhiy;
        ++pvstr;
        }

      if ( newhix != oldhix )
        {
        *pvstr = oldloy = newloy;
        ++pvstr;
        *pvstr = oldhix = newhix;
        ++pvstr;
        }
      else if ( newloy != oldloy )
        {
        *pvstr = oldloy = newloy;
        ++pvstr;
        }

      *pvstr = newlox;
      ++pvstr;
      *pvstr = '\0';
      fputs(vstr,stdout);
      break;
/*
***X-Windows.
*/
#ifdef V3_X11
      case X11:
      xbuf[xncrd].x = ix;
      xbuf[xncrd].y = gpsnpy-iy;
      ++xncrd;
      break;
#endif
/*
***Microsofts Windows.
*/
#ifdef WIN32
      case MSWIN:
      ms_buf[ms_ncrd].x = ix;
      ms_buf[ms_ncrd].y = gpsnpy-iy;
      ++ms_ncrd;
      break;
#endif
      }

    return(0);
}

/**********************************************************/
/*!******************************************************/

        short gpgtmc(
        char   *pektkn,
        double *px,
        double *py,
        bool    mark)

/*      Returnerar pek-tecken samt 2D-koordinat i modell-
 *      fönster koordinatsystemet.
 *
 *      In: pektkn => Pekare till pek-tecknet.
 *          px     => Pekare till X-koordinaten.
 *          py     => Pekare till Y-koordinaten.
 *          mark   => Pek-märke TRUE/FALSE
 *
 *      Ut:*pektkn => Pek-tecken.
 *         *px     => X-koordinat i 2D modellkoordinater.
 *         *py     => Y-koordinat i 2D modellkoordinater.
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 17/11/84 J. Kjellander
 *
 *       3/12/85  Returnera xy även vid REJECT, J. Kjellander
 *       30/12/85 mark, J. Kjellander
 *       16/1/87  ALFA terminal, R. Svedin
 *       8/1-95   Multifönster, J. Kjellander
 *
 ******************************************************!*/

  {

/*
***X-Windows.
*/
#ifdef V3_X11
    if ( gptrty == X11 ) return(wpgtmc(pektkn,px,py,mark));
#endif
/*
***Microsoft Windows.
*/
#ifdef WIN32
    return((short)msgtmc(pektkn,px,py,mark));
/*
***Övriga skärmar.
*/
#else
    if ( gptrty != ALFA )
      {
      short ix,iy;

      gpgtsc(pektkn,&ix,&iy,mark);

      *px = actvy.vywin[0] + (ix-viewpt[0])*(actvy.vywin[2]-actvy.vywin[0])/
                                   (viewpt[2]-viewpt[0]);
      *py = actvy.vywin[1] + (iy-viewpt[1])*(actvy.vywin[3]-actvy.vywin[1])/
                                   (viewpt[3]-viewpt[1]);
      }

    return(0);
#endif
  }

/********************************************************/
/*!******************************************************/

        short gpgtsc(
        char   *pektkn,
        short  *pix,
        short  *piy,
        bool    mark)

/*      Returnerar pek-tecken samt 2D-koordinat i ett
 *      koordinatsystem med origo i hela skärmens nedre
 *      vänstra hörn. Med mark = TRUE ritas pekmärke.
 *
 *      In: pektkn  => Pekare till char-variabel.
 *          pix     => Pekare till X-koordinat.
 *          piy     => Pekare till Y-koordinat.
 *          mark    => Pek-märke TRUE/FALSE
 *
 *      Ut: *pektkn => Pek-tecken.
 *          *pix    => X-koordinat.
 *          *piy    => Y-koordinat.
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 6/11/84 J. Kjellander V550
 *
 *      30/12/85 mark, J. Kjellander
 *      12/1/86  IP_32, J. Kjellander
 *      4/3/86   Bytt getchar mot iggtch. R: Svedin
 *      6/10/86  Tagit bort REJECT, J. Kjellander
 *      7/1/87   Nya terminaler FT4600, ALFA, R. Svedin
 *      7/12/87  Ny terminal MG700 B. Doverud
 *      21/1/88  Nya terminaler T4207, MO2000, B. Doverud
 *      31/5/88  Ny terminal TDV25, B. Doverud
 *      16/10/86 CGI, J. Kjellander
 *      20/10/88 N220G ersätter TDV25, B. Doverud 
 *      19/10/89 CGI-mus, J. Kjellander
 *      8/1-95   Multifönster, J. Kjellander
 *
 ******************************************************!*/

 {
    short tmpx,tmpy;
    char  ixhi,ixlo,iyhi,iylo,gintrm;
    v2int win_id;


    switch ( gptrty )
      {
/*
***X11.
*/
#ifdef V3_X11
      case X11:
      wpgtsc(mark,pektkn,pix,piy,&win_id);
      break;
#endif
/*
***Microsoft Windows.
*/
#ifdef WIN32
      case MSWIN:
      msgtsc(mark,pektkn,pix,piy,&win_id);
      break;
#endif
/*
***Tek-4010 terminaler.
*/
      case LVT100:
      case V550:
      case MG400:
      case MG420:
      case FT4600:
      case MG700:
      case T4207:
      case MO2000:
      case N220G:
      case MX7250:
      case MSCOLOUR:
      case MSMONO:
      gp4010();
      putchar(13) ;                      /* <CR> */
      putchar(31) ;                      /* <US> */
      putchar(27) ;                      /* <ESC> */
      putchar(26) ;                      /* <SUB> */
      igflsh();                          /* Töm buffer */

      *pektkn = iggtch();
      ixhi = iggtch();
      ixlo = iggtch();
      iyhi = iggtch();
      iylo = iggtch();
loop:                         /* Loopa runt och skippa tecken tills */
      gintrm = iggtch();      /* gin-terminator = <CR>, det kan ju  */
                              /* ligga fler än de 6 önskade tecknen */
      if (gintrm == '\n')     /* i input-bufferten !                */
         {
         *pix = 32*(ixhi-32) + ixlo-32;
         *piy = 32*(iyhi-32) + iylo-32;
         }
      else
         {
         *pektkn = ixhi;
         ixhi = ixlo;
         ixlo = iyhi;
         iyhi = iylo;
         iylo = gintrm;
         goto loop;
         }

      gpansi();
/*
***Om FACIT vrid koordinatsystemet 90 grader.
*/
      if ( gptrty == FT4600 )
        {
        tmpx = *piy;
        tmpy = gpgnpy-(*pix);
        *pix = tmpx;
        *piy = tmpy;
        }

      if ( mark ) gpdpmk(*pix,*piy);
      break;
/*
***Alfanumerisk terminal.
*/
      case ALFA:
      *pix = 0;
      *piy = 0;
      *pektkn = 'i';
      break;
      }
/*
***Slut.
*/
    return(0);
 }

/********************************************************/
/*!******************************************************/

        short gpdpmk(
        short ix,
        short iy)

/*      Ritar pekmärke och uppdaterar pekmärkes-listan.
 *
 *      In: ix,iy => Pekmärkets skärmposition.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C) microform ab 30/12/85 J. Kjellander
 *
 ******************************************************!*/
{

   gpspen(1);
   gpdmrk(ix,iy,PLUS,TRUE);

   if ( npmk < PMKMAX-1 )
     {
     pmkx[npmk] = ix;
     pmky[npmk] = iy;
     ++npmk;
     }

   return(0);
}
/********************************************************/
/*!******************************************************/

        short gpepmk()

/*      Suddar alla pekmärken och nollställer pekmärkes-
 *      listan.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C) microform ab 30/12/85 J. Kjellander
 *
 ******************************************************!*/
{
   register short i;

   for ( i=0; i<npmk; ++i ) gpdmrk(pmkx[i],pmky[i],PLUS,FALSE);

   npmk = 0;

   return(0);
}
/********************************************************/
/*!******************************************************/

        short gpdmrk(
        short ix,
        short iy,
        short typ,
        bool draw)

/*      Ritar/suddar grafiska markeringar.
 *
 *      In: ix,iy = Position på skärmen
 *          typ   = Typ av markering
 *          draw  = Rita/sudda
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C) microform ab 1/9 Ulf Johansson
 *
 *      30/12/85 PLUS, J. Kjellander
 *      11/2/86  DOT, J. Kjellander
 *      11/2/86  Ny metod för suddning, J. Kjellander
 *      19/3/86  gpitoa(), J. Kjellander
 *      7/1/87   Nya terminaler FT4600, ALFA, R. Svedin
 *      7/12/87  Ny terminal MG700 B. Doverud
 *      21/1/88  Nya terminaler T4207, MO2000, B.Doverud
 *      31/5/88  Ny terminal TDV25, B. Doverud
 *      20/10/88 N220G ersätter TDV25, B. Doverud 
 *      26/10/88 CGI, J. Kjellander
 *      29/6/92  X11, J. Kjellander
 *      8/11/93  actpen, J. Kjellander
 *      1996-12-11, MSWIN,ix+1 etc., J.Kjellander
 *
 ******************************************************!*/
{
     register short i;
     register char *pvstr;


     if (draw) 
       {
       switch (typ)
          {
/*
***Storheten punkt.
*/
          case KRYSS:
          i = 2;
          gpmvsc(ix-i,iy-i);
          gpdwsc(ix+i,iy+i);
          gpmvsc(ix-i,iy+i);
          gpdwsc(ix+i,iy-i);
          break;
/*
***Highlight-märke.
*/
          case DIAMANT:
          i = 4;
          gpmvsc(ix-i,iy);
          gpdwsc(ix,iy+i);
          gpdwsc(ix+i,iy);
          gpdwsc(ix,iy-i);
          gpdwsc(ix-i,iy);
          break;
/*
***Pek-märke.
*/ 
          case PLUS:
          i = 1;
          gpmvsc(ix-i,iy);
          gpdwsc(ix+i,iy);
          gpmvsc(ix,iy-i);
          gpdwsc(ix,iy+i);
          break;
/*
***Raster-prick.
*/
          case DOT:
          switch ( gptrty )
            {
            case LVT100:
            pvstr = vstr;
            *pvstr++ = 27;
            *pvstr++ = ':';
            pvstr = gpitoa(pvstr,ix);
            *pvstr++ = ';';
            pvstr = gpitoa(pvstr,iy);
            *pvstr++ = ';';
            if ( actpen == 0 ) *pvstr++ = '1';
            else               *pvstr++ = '0';
            *pvstr++ = ';';
            *pvstr++ = '0';
            *pvstr++ = 'p';
            *pvstr = '\0';
            fputs(vstr,stdout);
            break;

            case V550:
            case MG400:
            case MG420:
            case FT4600:
            case MG700:
            case MO2000:
            gp4010();
            putchar(28);       /* <FS> */
            gpdwsc(ix,iy);
            break;

            case T4207:
            case N220G:
            case MX7250:
            gp4010();
            putchar(28);       /* <FS> */
            gpdwsc(ix,iy);
            putchar(31);       /* <US> */
            break;

            case MSMONO:
            case MSCOLOUR:
            case X11:
            gpmvsc(ix,iy);
            gpdwsc(ix,iy); 
            break;

            case MSWIN:
            gpmvsc(ix,iy);
            gpdwsc(ix+1,iy+1); 
            break;

            case ALFA:
            return(0);
            break;
            }
         break;
         }
     }
/*
***Suddning.
*/
   else
     {
     if ( actpen != 0 ) gpspen(0);
     return(gpdmrk(ix,iy,typ,TRUE));
     }
/*
***Om X11 eller WIN32, töm buffer.
*/
   if ( gptrty == X11  ||
        gptrty == MSWIN ) gpflsh();

   return(0);
}

/********************************************************/
/********************************************************/

        short gpspen(
        short pen)

/*      Sätter om aktiv penna. För skärmar med färg
 *      ändras aktiv färg.
 *
 *      In: pen = Pennummer.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 26/1/86 J. Kjellander
 *
 *      30/10/86 penna != 0, J. Kjellander
 *      7/1/87   Nya terminaler FT4600, ALFA, R. Svedin
 *      7/12/87  Ny terminal MG700 B. Doverud
 *      21/1/88  Nya terminaler T4207, MO2000, B. Doverud
 *      31/5/88  Ny terminal TDV25, B. Doverud
 *      20/10/88 N220G ersätter TDV25, B. Doverud 
 *      26/10/88 CGI, J. Kjellander
 *      12/2/91  Max 12 pennor CGI, J. Kjellander
 *      8/3/91   MSKERMIT, J. Kjellander
 *      3/3/92   CGI-gpflsh(), J. Kjellander
 *      27/6/92  X11, J. Kjellander
 *      10/11/95 WIN32, J. Kjellander
 *
 ********************************************************/

{

/*
***Ställ om skärmen.
*/
    switch (gptrty)
      {
/*
***Luxor VT100.
*/
      case LVT100:
      break;
/*
***Div. Tektronix-kompatibla.
*/
      case V550:
      case MG400:
      case MG420:
      case FT4600:
      case MG700:
      case MO2000:
      case N220G:
      case MSMONO:
      gp4010();
      if ( pen == 0 ) fputs(pen0,stdout);
      else            fputs(pen1,stdout);
      break;
/*
***Tektronix 4207 och kompatibla.
*/
      case T4207:
      case MX7250:
      gp4010();
      switch ( pen )
        {
        case 0: fputs(pen0,stdout); break;
        case 1: fprintf(stdout,"%s%d",pen1,1); break;
        case 2: fprintf(stdout,"%s%s",pen1,"="); break;
        case 3: fprintf(stdout,"%s%d",pen1,9); break;
        case 4: fprintf(stdout,"%s%d",pen1,5); break;
        case 5: fprintf(stdout,"%s%d",pen1,2); break;
        case 6: fprintf(stdout,"%s%d",pen1,3); break;
        case 7: fprintf(stdout,"%s%d",pen1,4); break;
        case 8: fprintf(stdout,"%s%d",pen1,8); break;
        case 9: fprintf(stdout,"%s%d",pen1,7); break;
        case 10: fprintf(stdout,"%s%d",pen1,6); break;
        case 11: fprintf(stdout,"%s%d",pen1,5); break;
        default: fprintf(stdout,"%s%s",pen1,"?"); break;
        }
      break;
/*
***X11.
*/
#ifdef V3_X11
      case X11:
      wpspen(pen);
      break;
#endif
/*
***Microsoft Windows.
*/
#ifdef WIN32
      case MSWIN:
      wpspen(pen);
      break;
#endif
/*
***MSKERMIT med färg.
*/
      case MSCOLOUR:
      gp4010();
      switch ( pen )
        {
        case 0:  fputs(pen0,stdout); break;
        case 1:  fputs(pen1,stdout); break;
        case 2:  fputs("\033[31m",stdout); break;
        case 3:  fputs("\033[32m",stdout); break;
        case 4:  fputs("\033[34m",stdout); break;
        default: fputs(pen1,stdout); break;
        }
      break;
/*
***Alfanumerisk terminal.
*/
      case ALFA:
      return(0);
      break;
      }
/*
***Aktiv penna.
*/
    actpen = pen;

    return(0);
}

/********************************************************/
/********************************************************/

        short gpsbrush(
        short pen)

/*      Sets active WIN32-brush.
 *
 *      In: pen = Color number.
 *
 *      (C)microform ab 1999-12-13 J. Kjellander
 *
 ********************************************************/

{
#ifdef WIN32
   SelectObject(ms_dc,msgbrush((int)pen));
   SelectObject(ms_bmdc,msgbrush((int)pen));
#endif

    return(0);
}

/********************************************************/
/********************************************************/

        short gpswdt(
        double width)

/*      Sätter om aktiv bredd.
 *
 *      In: Width = Önskad bredd eller 0.0 för minsta möjliga.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1997-12-25 J. Kjellander
 *
 ********************************************************/

{

/*
***X11.
*/
#ifdef V3_X11
      if ( gptrty == X11 ) wpswdt(GWIN_MAIN,width);
      else                 return(0);
#endif

/*
***Microsoft Windows.
*/
#ifdef WIN32
      wpswdt(GWIN_MAIN,width);
#endif

    return(0);
}

/********************************************************/
