/**********************************************************************
*
*    gp17.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gpdrxh();    Draw xhatch
*    gpdlxh();    Erase xhatch
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
#include "../include/GP.h"

extern tbool  nivtb1[];
extern double x[],y[];
extern char   a[];
extern short  actpen;
extern int    ncrdxy;
extern double k1x,k1y,k2x,k2y,ritskl;

/*!******************************************************/

        short gpdrxh(
        GMXHT *xhtpek,
        gmflt xyvek[],
        DBptr la,
        short drmod)

/*      Ritar ett snitt
 *
 *      IN: xhtpek => Adress till snittstruktur
 *          xyvek  => Vektor med x och y_koordinater
 *          la     => Logisk adress i GM.
 *          drmod  => Ritmode.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          GP0012 => Displayfilen full.
 *
 *      (C)microform ab 25/8-85 Ulf Johansson
 *
 *      REVIDERAD:
 *  
 *      24/9-85  Hitpunkt, Ulf Johansson
 *      16/10-85 HUGE, J. Kjellander
 *      27/1/86  Penna, B. Doverud
 *      29/9/86  Ny nivåhant. R. Svedin
 *      14/10/86 ritskl, J. Kjellander
 *      15/10/86 drmod, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *  
 ******************************************************!*/

  {
    int    k,i,j,n;
    double min,try,x1,y1,x2,y2;
    double tmpx[PLYMXV],tmpy[PLYMXV];
    char   tmpa[PLYMXV];
    GMLIN linje;

/*
***Släckt nivå eller är blankad ? 
*/
    if (nivtb1[xhtpek->hed_xh.level] || xhtpek->hed_xh.blank )
         return(0);
/*
***Ev. ritskalning.
*/
    if ( xhtpek->fnt_xh > 0 ) xhtpek->lgt_xh /= ritskl;
/*
***Generera vektorer.
*/
    i = 0;
    n = 4*xhtpek->nlin_xh;
    k = -1;

    while (i < n)
      {
      x[++k] = xyvek[ i++ ];
      y[k]   = xyvek[ i++ ];
      a[k]   = 0;
      x[++k] = xyvek[ i++ ];
      y[k]   = xyvek[ i++ ];
      a[k]   = (VISIBLE | ENDSIDE);
      }
    ncrdxy = k + 1;
/*
***Ev. klippning.
*/
    if ( drmod == CLIP )
      {
      ncrdxy = 0;
      klpply(-1,&k,x,y,a);
      ncrdxy = k+1;
      }
/*
***Ev. ritning.
*/
    if ( drmod == DRAW )
      {
      if ( xhtpek->hed_xh.pen != actpen ) gpspen(xhtpek->hed_xh.pen);

      i = 0;
      n = 2*xhtpek->nlin_xh;
      linje.fnt_l = xhtpek->fnt_xh;
      linje.lgt_l = xhtpek->lgt_xh;
      min = 1E30;
      j = -1;

      while ( i<n )
         {
         tmpx[0] = x[i];
         tmpy[0] = y[i];
         tmpa[0] = a[i++];
         tmpx[1] = x[i];
         tmpy[1] = y[i];
         tmpa[1] = a[i++];

         if ( klplin((short)-1,tmpx,tmpy,tmpa) )
           {
           k = -1;
           gpplli(&linje,&k,tmpx,tmpy,tmpa); 
           drwply(k,tmpx,tmpy,tmpa,TRUE);

           x2 = k1x + k2x*tmpx[0];
           y2 = k1y + k2y*tmpy[0];
           if ((try = x2*x2 + y2*y2) < min)
             {
             x1 = tmpx[0];
             y1 = tmpy[0];
             min = try;
             j = i;
             }

           x2 = k1x + k2x*tmpx[1];
           y2 = k1y + k2y*tmpy[1];
           if ((try = x2*x2 + y2*y2) < min)
             {
             x1 = tmpx[1];
             y1 = tmpy[1];
             min = try;
             j = i;
             }
           }
         }
      }
/*
***Ev. lagring i df.
*/
      if ( drmod == DRAW && xhtpek->hed_xh.hit )
        {
        if (j >= 0)
          {
          x[ 0 ] = x1;
          y[ 0 ] = y1;
          a[ 0 ] = 0;
          if (!stoply(0,x,y,a,la,XHTTYP))
                         return(erpush("GP0012",""));
          }
        }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpdlxh(
        GMXHT *xhtpek,
        gmflt xyvek[],
        DBptr la)

/*      Suddar snitt
 *
 *      IN: xhtpek => Adress till snittstruktur
 *          xyvek  => Vektor med x och y_koordinater
 *          la     => Logisk adress i GM.
 *
 *      UT: Inget.
 *
 *      FV:     0 => Ok.
 *
 *      (C)microform ab 25/8-85 Ulf Johansson
 *
 *      REVIDERAD:
 *  
 *      3/9-85   Div, Ulf Johansson
 *      14/10/86 ritskl, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      24/3/87  Släckt nivå, J. Kjellander
 *      31/10/88 Bug vid tilldeln. av aray, R. Svedin 
 *
 ******************************************************!*/

{
    int     k,i,j0,j1,n;
    DBetype typ;
    GMLIN   linje;

/*
***Sudda ur DF.
*/
    if (fndobj(la,XHTTYP,&typ))
      {
      gphgen(la,HILIINVISIBLE);          /* Sudda highlight */
      remobj();                          /* Sudda ur DF */
      }
/*
***Släckt nivå eller är blankad ? 
*/
    if (nivtb1[xhtpek->hed_xh.level] || xhtpek->hed_xh.blank )
         return(0);
/*
***Ev. ritskalning.
*/
    if ( xhtpek->fnt_xh > 0 ) xhtpek->lgt_xh /= ritskl;

    linje.fnt_l = xhtpek->fnt_xh;       /* font */
    linje.lgt_l = xhtpek->lgt_xh;       /* strecklängd */
    i = 0;
    n = 4*xhtpek->nlin_xh;
    j0 = 0;
    j1 = 1;

    while (i < n)
      {
      x[j0] = xyvek[ i++ ];
      y[j0] = xyvek[ i++ ];
      a[j0] = 0;
      x[j1] = xyvek[ i++ ];
      y[j1] = xyvek[ i++ ];
      a[j1] = (VISIBLE | ENDSIDE);

      if (klplin(-1,x,y,a))
        {
        k = -1;
        gpplli(&linje,&k,x,y,a); 
        drwply(k,x,y,a,FALSE);
        }
      }

    return(0);
}

/********************************************************/




