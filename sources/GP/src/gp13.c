/**********************************************************************
*
*    gp13.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gpgtla();    DBptr for closest object in displayfile
*    gpgmla();    DBptr for all objects in a rectangle
*
*    fndobj();    DBptr for current object
*    remobj();    Delete current object from displayfile
*    drwobj();    Draw/erase current object
*
*    stoply();    Store polyline in displayfile
*    drwply();    Draw/erase polyline outline
*    fillply();   Fill/erase polyline area
*
*    gphgen();    Highlight on/off for one object
*    gphgal();    Highlight on/off for all objects
*    gperhg();    Erase all higlight markers
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
#include <math.h>

extern short   gptrty,actpen;
extern gmint   dfpek,dfcur;
extern double  k1x,k1y,k2x,k2y;
extern DFPOST *df;
extern V3MSIZ  sysize;

/*!******************************************************/

        bool fndobj(
        DBptr    la,
        DBetype  typmsk,
        DBetype *typ)

/*      Söker grafiskt objekt i displayfilen (df) som stämmer
 *      med angiven logisk adress (la) och typmask.
 *
 *      In: la:         Logisk adress
 *          typmsk:     Typmask för ev begränsad sökning
 *
 *      Ut: typ:        Typ för den funna storheten.
 *
 *      FV: fndply:     TRUE = sökningen lyckades.
 *
 *      (C) microform ab 1/8-85 Ulf Johansson
 *
 ******************************************************!*/
{
     gmint i;

     i = dfcur;
/*
***Är det aktuellt objekt som sökes ?
*/
     if (i >= 0)
          if ((df[ i+1 ].la == la) &&
              ((df[ i ].hdr.type & typmsk) > 0))
            {
            *typ = df[ i ].hdr.type;
            return(TRUE);
            }
/*
***Sök igenom hela displayfilen.
*/
     i = 0;
     while (i < dfpek)
       {
       if ((df[ i+1 ].la == la) &&
           ((df[ i ].hdr.type & typmsk) > 0))
         {
         dfcur = i;                /* Nytt aktuellt objekt */
         *typ = df[ i ].hdr.type;
         return(TRUE);
         }
       i += 3 + df[ i ].hdr.nvec;
       }

     return(FALSE);

 }

/********************************************************/
/********************************************************/

   bool stoply(
        int     k,
        double  x[],
        double  y[],
        char    a[],
        DBptr   la,
        DBetype tp)

/* 
 *      Lagrar en polyvektor i displayfilen.
 *
 *      IN:
 *         k: Antal vektorer
 *         x: X-koordinater
 *         y: Y-koordinater
 *         a: Vektorstatus
 *         la: Pekare till geometriminnet (GM).
 *         tp: Typ av grafiskt objekt.
 *
 *      FV:
 *         stoply: Vektorn fick plats i displayfilen.
 *
 *      Felkod: GP0112 - Kan ej malloc()/realloc()
 *
 *      (C) microform ab 15/7-85 Ulf Johansson
 *
 *       28/11/89 Ny df-hantering, J. Kjellander
 *       13/7/04  Mesh, J.Kjellander, Örebro university
 *
 *********************************************************/
{
     gmint i,j;

/*
***Initiering.
*/
     j = dfpek;
/*
***Se till att plats finns.
*/
loop:
     if ( j+k >= sysize.df-3 )
       {
       if ( sysize.df == 0 )
         {
         if ( (df=(DFPOST *)v3mall((unsigned)(PLYMXV*sizeof(DFPOST)),
                                                       "DF")) == NULL )
           {
           erpush("GP0112","");
           errmes();
           return(FALSE);
           }
         else
           {
           sysize.df = PLYMXV;
           goto loop;
           }
         }
       else
         {
         if ( (df=(DFPOST *)v3rall((char *)df,
                  (unsigned)((sysize.df+PLYMXV)*sizeof(DFPOST)),"DF")) == NULL )
           {
           erpush("GP0112","");
           errmes();
           return(FALSE);
           }
         else
           {
           sysize.df += PLYMXV;
           goto loop;
           }
         }
       }
/*
***Lagra objektet.
*/
     dfcur = ++j;                        /* Objektet blir aktuellt */

     df[j].hdr.type = tp;              /* Objektet typ */
     df[j].hdr.nvec = k;               /* Antal vektorer */
     df[j].hdr.hili = 0;               /* Ingen highlight */
     df[++j].la = la;                  /* Adress i grafikminnet*/

     for (i = 0; i <= k; i++) {          /* Vektorerna */
          
          df[++j].vec.x = (short)(k1x + k2x*x[i]);
          df[ j ].vec.y = (short)(k1y + k2y*y[i]);
          df[ j ].vec.a = a[i];
     }

     dfpek = j;

     return(TRUE);
}
/********************************************************/
/********************************************************/

   bool remobj()

/* 
 *      Suddar aktuellt grafiskt objekt i displayfilen (df).
 *
 *      IN:
 *
 *      UT:
 *
 *      FV:
 *         stoply:     TRUE
 *
 *
 *      (C) microform ab 1/8-85 Ulf Johansson
 *
 *********************************************************/
{

     df[ dfcur+1 ].la = SUDDAD;
     dfcur = -1;                         /* Inget objektet är aktuellt */

     return(TRUE);
}
/********************************************************/
/********************************************************/

   bool drwobj(
        bool s)

/* 
 *      Ritar (suddar) aktuellt (dfcur) grafiskt objekt.
 *
 *      IN:
 *         s: TRUE = Rita (FALSE = Sudda).
 *
 *      UT:
 *
 *      FV: TRUE
 *
 *
 *      (C) microform ab 1/8-85 Ulf Johansson
 *
 *       12/2/86  Ny metod för suddning, J. Kjellander
 *       16/10/88 CGI, J. Kjellander
 *       16/6/92  X11, J. Kjellander
 *       1/11/93  actpen, J. Kjellander
 *
 *********************************************************/
{
     gmint i,j,k;

/*
***Initiering.
*/
     j = dfcur + 2;
     k = j + df[ dfcur ].hdr.nvec;
/*
***Rita.
*/
     if ( s )
       {
       for (i=j; i<=k; i++)
         {
         if ( (df[i].vec.a & VISIBLE) == VISIBLE) 
                    gpdwsc(df[i].vec.x,df[i].vec.y);
          else gpmvsc(df[i].vec.x,df[i].vec.y);
         }
       }
/*
***Sudda.
*/
     else
       {
       if ( actpen != 0 ) gpspen(0);
       return(drwobj(TRUE));
       }
/*
***Om X11, töm outputbufferten.
*/
#ifdef V3_X11
   if ( gptrty == X11 ) gpflsh();
#endif

/*
***Om Windows, töm outputbufferten.
*/
#ifdef WIN32
   gpflsh();
#endif

     return(TRUE);
}
/********************************************************/
/*!******************************************************/

        DBptr gpgtla(
        DBetype  typmsk,
        short    ix,
        short    iy,
        DBetype *typ,
        bool    *ends,
        bool    *right)


/*      Returnerar GM-adress för storhet närmast
 *      skärm-positionen ix, iy och med typ som
 *      stämmer med typmsk.
 *
 *      In: typmsk => Mask för ev. begränsad sökning
 *          ix,iy  => Koordinat i skärmkoordinater
 *          typ    => Pekare till DBetype-variabel.
 *
 *      Ut: *typ   => Typ för den funna storheten.
 *                    Odefinierad om ingen storhet
 *                    hittats.
 *          *ends  => Skärmpos. ligger närmast obj. slut
 *          *right => Skärmpos. ligger på obj. högra sida.
 *
 *
 *      FV: >= 0  => GM-adress för närmsta storhet.
 *            -1  => Ingen storhet hittat.
 *
 *      (C)microform ab 4/1-85 J. Kjellander
 *
 *      REVIDERAD:
 *
 *      19/6/85  Heltalsmetod, Ulf Johansson
 *      29/10/85 Endside-buggen fixad, U. Johansson
 *      17/11/85 Sid-buggen fixad, J. Kjellander
 *
 ******************************************************!*/

  {
    gmint i,n,j;
    int  imin,nmin,ilim,dist,mdist;
    int  t1=0,t2=0;
    int  ixt,iyt,idx,idy;
    char a=0;
    gmflt tt;


    mdist = 2000000000;
    imin = -1;   /* Pekare till närmsta objekt */
    nmin = 0;    /* Pekare till närmsta vektor */
/*
***Här börjar loopen för hela displayfilen.
*/
    for (i = 0; i < dfpek; i += 3 + df[ i ].hdr.nvec)
       {
       if (((df[ i ].hdr.type & typmsk) > 0) && (df[ i+1 ].la != SUDDAD))
         {
/*
***Objekt typ punkt.
*/
         if (df[ i ].hdr.nvec == 0 )
           { 
           ixt = (gmint)(ix - df[ i+2 ].vec.x);
           iyt = (gmint)(iy - df[ i+2 ].vec.y);
          
           if ((dist = ixt*ixt + iyt*iyt) < mdist)
             {
             mdist = dist;
             imin = i;
             a = df[ i+2 ].vec.a;
             }
           }
/*
***Objekt typ polyline.
*/
         else
           {
           ilim = i + 2 + df[ i ].hdr.nvec;
           for (n = i + 2; n < ilim; n++)
              {
              idx = df[ n+1 ].vec.x - df[ n ].vec.x;
              idy = df[ n+1 ].vec.y - df[ n ].vec.y;
              ixt = ix - df[ n ].vec.x;
              iyt = iy - df[ n ].vec.y;

              j = 0;
              if (idx != 0  ||  idy != 0)
                {
                if ((t1 = idx*ixt + idy*iyt) > 0) 
                  {                                     /* t >= 0 */
                  if (t1 >= (t2 = idx*idx + idy*idy))
                    {
                                                            /* t >= 1 */
                    ixt -= idx;
                    iyt -= idy;
                    j = 1;
                    }
                  else
                    {        
                                                         /* 0 < t < 1 */
                    ixt -= (idx*t1)/t2;
                    iyt -= (idy*t1)/t2;
                    j = 2;
                    }
                  }
                }
/*
***Är denna vektor närmast hittills?
*/            
              if ((dist = ixt*ixt + iyt*iyt) < mdist)
                {
                if ((df[ n+1 ].vec.a & VISIBLE) == VISIBLE)
                  {
                  mdist = dist;
                  imin = i;
                  nmin = n;
                  if (j == 0) a = df[ n ].vec.a;           /* t = 0 */
                  else if (j == 1) a = df[ n+1 ].vec.a;    /* t = 1 */
                  else if (2*t1 < t2) a = df[ n ].vec.a;   /* t < 0.5 */
                  else a = df[ n+1 ].vec.a;                /* t >= 0.5 */
                  }
                }
             }
          }
       }
    }
/*
***Avslutning.
*/
    if (imin != -1)
      {
      dfcur = imin;                         /* Objektet blir aktuellt */
      *typ = df[ imin ].hdr.type;
/*
***Om polylinje, beräkna ände och sida.
*/
      if ( df[imin].hdr.nvec != 0 )
        {
        *ends = ((a & ENDSIDE) != 0);
        *right = TRUE;

        idx = df[nmin+1].vec.y - df[nmin].vec.y;
        idy = df[nmin].vec.x - df[nmin+1].vec.x;
        ixt = ix - df[nmin].vec.x;
        iyt = iy - df[nmin].vec.y;
        tt = idx*idx + idy*idy;

        if ( tt > 0.0 )
          {
          tt = SQRT(tt);
          tt = idx*ixt/tt + idy*iyt/tt;
          if ( tt < 0.0 ) *right = FALSE;
          }
        }
      return(df[ imin+1 ].la);
      }

    else return(-1);
  }
     
/********************************************************/
/*!******************************************************/

        short gpgmla(
        short   ix1,
        short   iy1,
        short   ix2,
        short   iy2,
        short   mode,
        bool    hl,
        short  *nla,
        DBetype typvek[],
        DBptr   lavek[])

/*      Returnerar LA för alla DF-objekt inom/utom ett
 *      fönster.
 *
 *      In: ix1,iy1  =  Fönstrets nedre vänstra hörn.
 *          ix2,iy2  =  Fönstrets övre högra hörn.
 *          mode     =  0 = Alla helt inom
 *                      1 = Alla helt eller delvis inom
 *                      2 = Alla helt utom
 *                      3 = Alla helt eller delvis utom
 *          hl       = TRUE => Highligting
 *          *nla     =  Max antal objekt som får returneras.
 *          typvekÄ0Å=  Typmask för ev begränsad sökning
 *
 *      Ut: *nla     =  Antal objekt som uppfyller villkoren.
 *          *typvek  =  Typ för nla objekt.
 *          *lavek   =  LA för nla objekt.
 *
 *      FV:  0
 *
 *      (C) microform ab 17/11/88 J. Kjellander
 *
 ******************************************************!*/

 {
    gmint i,j,k;
    short maxobj,nv,x,y,nhit,nin,nut;
    DBetype orgtyp;
    bool  inside,outside,hit;
    double win[4],vec[4];

/*
***Initiering.
*/
    win[0] = ix1; win[1] = iy1;
    win[2] = ix2; win[3] = iy2;
    orgtyp = typvek[0];
    nhit = 0;
    maxobj = *nla;
    i = 0;
/*
***Här börjar sökning genom hela displayfilen.
*/
    while ( i < dfpek )
      {
/*
***Sök upp nästa icke suddade objekt av rätt typ.
*/
      if ( df[i+1].la != SUDDAD  &&  (df[i].hdr.type & orgtyp) > 0 )
        {
        nv = df[i].hdr.nvec;
/*
***Klassificera objekt av typen "punkt".
*/
        if ( nv == 0 )
          {
          x = df[i+2].vec.x;  y = df[i+2].vec.y;
          inside = ( (x > ix1) && (x < ix2) && 
                     (y > iy1) && (y < iy2) );
          outside  = !inside;
          }
/*
***Klassificera objekt av typen "polyline".
*/
        else
          {
          j = i+2;
          k = j+nv;
          nin = nut = 0;
          for ( ; j<k; ++j )
            {
            if ( (df[j+1].vec.a & VISIBLE) == VISIBLE )
              {
              vec[0] = df[j].vec.x; vec[1] = df[j].vec.y;
              vec[2] = df[j+1].vec.x; vec[3] = df[j+1].vec.y;
              switch ( klp(vec,win) )
                {
                case -1: ++nut; break;
                case  0: ++nin; break;
                default: ++nut; ++nin; break;
                }
              }
            }
          inside = outside = FALSE;
          if ( nin > 0 ) inside = TRUE;
          if ( nut > 0 ) outside = TRUE;
          }
/*
***Uppfyller objektet de ställda kraven ?
*/
        hit = FALSE;
        switch ( mode)
          {
          case 0: if ( inside  &&  !outside ) hit = TRUE; break;
          case 1: if ( inside ) hit = TRUE; break;
          case 2: if ( outside  &&  !inside ) hit = TRUE; break;
          case 3: if ( outside ) hit = TRUE; break;
          }
        if ( hit )
          {
          if ( nhit == maxobj ) goto end;
          lavek[nhit] = df[i+1].la;
          typvek[nhit] = df[i].hdr.type;
#ifdef V3_X11
          if ( hl ) wphgen(GWIN_ALL,lavek[nhit],TRUE);
#else
#ifdef WIN32
          if ( hl ) wphgen(GWIN_ALL,lavek[nhit],TRUE);
#else
          if ( hl ) gphgen(lavek[nhit],(short)1);
#endif
#endif
          nhit++;
          }
        }
/*
***Nästa objekt i DF.
*/
      i += (3 + df[i].hdr.nvec);
      }
/*
***Slut.
*/
end:
     *nla = nhit;
     return(0);

 }

/********************************************************/
/********************************************************/

   bool drwply(
        int    k,
        double x[],
        double y[],
        char a[],
        bool s)

/* 
 *      Ritar (suddar) en polyvektor på bildskärmen.
 *
 *      IN:
 *         k: Antal vektorer
 *         x: X-koordinater
 *         y: Y-koordinater
 *         a: Vektorstatus
 *         s: TRUE = Rita (FALSE = Sudda).
 *
 *      UT:
 *
 *      FV: TRUE
 *
 *
 *      (C) microform ab 15/7-85 Ulf Johansson
 *
 *       12/2/86  Ny metod för suddning, J. Kjellander
 *       16/10/88 CGI, J. Kjellander
 *       16/6/92  X11, J. Kjellander
 *       1/11/93  actpen, J. Kjellander
 *
 *********************************************************/

  {
   int i;

/*
***Rita.
*/
   if ( s )
     {
     for (i=0; i<=k; i++)
       {
       if ( (a[i] & VISIBLE) == VISIBLE ) 
                         gpdwsc((short)(k1x + k2x*x[ i ]),
                         (short)(k1y + k2y*y[ i ]));
        else gpmvsc((short)(k1x + k2x*x[ i ]),
                         (short)(k1y + k2y*y[ i ]));
       }
     }
/*
***Sudda. Här gör vi return direkt utan att tömma buffrar
***eftersom annars buffrar kommer att tömmas två gånger varav
***ena gången tom.
*/
   else
     {
     if ( actpen != 0 ) gpspen(0);
     return(drwply(k,x,y,a,TRUE));
     }
/*
***Om X11, töm outputbufferten.
*/
#ifdef V3_X11
   if ( gptrty == X11 ) gpflsh();
#endif
/*
***Om Windows, töm outputbufferten.
*/
#ifdef WIN32
   gpflsh();
#endif

   return(TRUE);
  }

/********************************************************/
/********************************************************/

   bool fillply(
        int    k,
        double x[],
        double y[],
        short  pen)

/* 
 *      Fill/erase area enclosed by polygon.
 *
 *      In:
 *         k:   Number of lines (vertices - 1)
 *         x:   X-coordinates
 *         y:   Y-coordinates
 *         pen: Color number
 *
 *      (C)microform ab 1999-12-13 J.Kjellander
 *
 *********************************************************/

  {
/*
***X version.
*/
#ifdef V3_X11
   int    i;
   XPoint p[PLYMXV+1];

extern Display *xdisp;
extern Window   xgwin;
extern Pixmap   xgmap;
extern GC       gpxgc;
extern short    gpsnpy;

/*
***What color ?
*/
   if ( pen != actpen ) gpspen(pen);
/*
***Make XPoint array.
*/
   for ( i=0; i<=k; ++i )
     {
     p[i].x = (short)(k1x + k2x*x[i]);
     p[i].y = (short)(gpsnpy - (k1y + k2y*y[i]));
     }

   XFillPolygon(xdisp,xgwin,gpxgc,p,(int)(k+1),Nonconvex,CoordModeOrigin);
   XFillPolygon(xdisp,xgmap,gpxgc,p,(int)(k+1),Nonconvex,CoordModeOrigin);
   gpflsh();
#endif

/*
***WIN32 version.
*/
#ifdef WIN32
   int   i;
   POINT p[PLYMXV+1];

extern short gpsnpy;

/*
***What color ?
*/
   gpsbrush(pen);
/*
***Make POINT array.
*/
   for ( i=0; i<=k; ++i )
     {
     p[i].x = (long)(k1x + k2x*x[i]);
     p[i].y = (long)(gpsnpy - (k1y + k2y*y[i]));
     }
/*
***Fill polygon.
*/
   Polygon(ms_dc,p,(int)(k+1));
   Polygon(ms_bmdc,p,(int)(k+1));
#endif

   return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        short gphgen(
        DBptr la,
        DBint mrktyp)

/*      "Highlightar" ett grafiskt objekt. Anropas av
 *       getidt() med mrktyp = 1 samt av alla gp-sudda-rutiner
 *       med mrktyp = 0.
 *
 *      IN:
 *         la:         Logisk adress
 *         mrktyp = 0: Sudda   (HILIINVISIBLE)
 *                  1: Diamant (HILIDIAMOND)
 *
 *      (C) microform ab 1/9 Ulf Johansson
 *
 *      REVIDERAD:
 *
 *         13/9-85 Ulf Johansson
 *         8/11/93 Snabb suddning, J. Kjellander
 *
 ******************************************************!*/

 {
   gmint i,j;
   short ix=0,iy=0,t;
   DBetype typ;
   bool b;

/*
***Leta upp objektet i DF.
*/
   if ( fndobj(la,ALLTYP,&typ) )
     {
/*
***Om suddning begärts och objektet inte är highlightat
***går det snabbt. Likaså om highligt begärts och objektet
***redan är det.
*/
     if ( mrktyp == df[dfcur].hdr.hili ) return(0);
/*
***Objektet är highligtat och skall suddas eller är det inte men
***skall bli det.
*/
     i = df[ dfcur ].hdr.nvec;
     j = dfcur + 2;
/*
***Highlightmärke ritas normalt med penna 1.
*/
     if ( actpen != 1 ) gpspen(1);
/*
***Leta upp rätt vektor och därmed märkets placering.
*/
     if ( i == 0 )
       {
       ix = df[ j ].vec.x;
       iy = df[ j ].vec.y;
       }
     else
       {
       i /= 2;
       j += i;
       while ( i >= 0 )
         {
         if ( (df[ j+1 ].vec.a & VISIBLE) == VISIBLE )
           {
           ix = (df[ j+1 ].vec.x + df[ j ].vec.x)/2;
           iy = (df[ j+1 ].vec.y + df[ j ].vec.y)/2;
           break;
           }
         i--;
         j++;
         }
       }

       if ( i >= 0 )
         {
         if ( !(b = (mrktyp != HILIINVISIBLE)) )
           {  /* b = rita/sudda */
           mrktyp = df[ dfcur ].hdr.hili;
           df[ dfcur ].hdr.hili = HILIINVISIBLE;
           }
         else df[ dfcur ].hdr.hili = (char)mrktyp;

         switch ( mrktyp )
           {
           case HILIDIAMOND:
           t = DIAMANT;                   /* Diamond */
           break;

           default:
           t = NOMARK;
           break;
           }
           gpdmrk(ix,iy,t,b);
           return(0);
        }
     }
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short gphgal(
        DBint mrktyp)

/*      "Highlightar" alla objekt i DF. Om suddning
 *      begärts suddas även pekmärken.
 *
 *      In: mrktyp = 0: Sudda
 *                   1: Diamant
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C) microform ab 1/9 Ulf Johansson
 *
 *      30/12/85 Pekmärke, J. Kjellander
 *      16/11/88 gperhg(), J. Kjellander
 *
 ******************************************************!*/
{
     gmint  i,j,n,l;
     short ix=0,iy=0;
     int t;
     bool b;

/*
***Om suddning begärts, sudda med gperhg() som är
***snabbare och sudda även ev. pekmärken.
*/
     if ( mrktyp == HILIINVISIBLE )
       {
       gperhg();
       gpepmk();
       return(0);
       }
/*
***Rita highlightmärken.
*/
     b = (mrktyp != HILIINVISIBLE);       /* b = rita/sudda */
     n = 0;
     while (n < dfpek) {

          j = n;
          i = df[ j++ ].hdr.nvec;

          if (df[ j++ ].la != SUDDAD) {

               if (i == 0) {
                    l = 0;
                    ix = df[ j ].vec.x;
                    iy = df[ j ].vec.y;
               } else {
                    l = i/2;
                    j += l;
                    while (l >= 0) {
                         if ((df[ j+1 ].vec.a & VISIBLE) == VISIBLE) {
                              ix = (df[ j+1 ].vec.x + df[ j ].vec.x)/2;
                              iy = (df[ j+1 ].vec.y + df[ j ].vec.y)/2;
                              break;
                         }
                         l--;
                         j++;
                    }
               }
               if (l >= 0) {

                    if (b)
                         df[ n ].hdr.hili = (char)mrktyp;
                    else {
                         mrktyp = df[ n ].hdr.hili;
                         df[ n ].hdr.hili = HILIINVISIBLE;   /* Sudda */
                    }
     
                    switch (mrktyp) {
                         case HILIDIAMOND:
                              t = DIAMANT;                   /* Diamond */
                              break;
                         default:
                              t = NOMARK;
                              break;    
                    }

                    gpdmrk(ix,iy,(short)t,b);

               }
          }

          n += i + 3;
     }
   return(0);
}

/********************************************************/
/*!******************************************************/

        short gperhg()

/*      Suddar alla highligtmärken. Denna rutin söker
 *      igenom DF och bryr sig bara om objekt som är
 *      highligtade. Den ursprungliga metoden att sudda
 *      alla märken med gphgal(0) är mycket långsammare
 *      eftersom den "släcker" highligtmärken även på
 *      objekt som inte har några. gphgal() skall bara
 *      användas för att tända highligtmärken.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C) microform ab 16/11/88 J. Kjellander
 *
 *      1997-04-10 WIN32, J.Kjellander
 *
 ******************************************************!*/
 {
   int  i,j,n,l;
   short ix=0,iy=0;

/*
***Börja på första posten i DF.
*/
   n = 0;
/*
***Sök igenom hela DF sekvensiellt.
*/
   while ( n < dfpek )
     {
/*
***Är objektet icke suddat och dessutom highligtat ?
*/
     if ( df[n+1].la != SUDDAD  &&  df[n].hdr.hili > 0 )
       {
       j = n+2;
       i = df[n].hdr.nvec;
       if ( i == 0 )
         {
         l = 0;
         ix = df[j].vec.x;
         iy = df[j].vec.y;
         }
       else
         {
         l = i/2;
         j += l;
         while ( l >= 0 )
           {
           if ( (df[j+1].vec.a & VISIBLE) == VISIBLE )
             {
             ix = (df[j+1].vec.x + df[j].vec.x)/2;
             iy = (df[j+1].vec.y + df[j].vec.y)/2;
             break;
             }
           l--;
           j++;
           }
         }
       if ( l >= 0 )
         {
         df[n].hdr.hili = 0;
         gpdmrk(ix,iy,(short)DIAMANT,FALSE);
         }
       }
/*
***Nästa objekt i DF.
*/
     n += (df[n].hdr.nvec + 3);
     }

#ifdef V3_X11
   if ( gptrty == X11 ) wperhg();
#endif
#ifdef WIN32
   wperhg();
#endif
/*
***Slut.
*/
    return(0);
 }

/********************************************************/
