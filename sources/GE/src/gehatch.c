/*!******************************************************************/
/*  File: gehatch.c                                                 */
/*  ===============                                                 */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GEhatch2D()  2D hatching                                        */
/*  GEarea2D()   2D area/center_of_gravity                          */
/*  GEint_2Dlb() Intersect 2D line with 2D boundary                 */
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://varkon.sourceforge.net                             */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/*  (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se    */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*
***Av någon orsak visar det sig att geo820() inte tål
***optimering under WIN32. Alla snitt åt höger skapas
***men när det är dags att gå åt vänster tar geo820()
***inte den nya startpunkten (px,py) utan återanvänder
***sista högra linjens (px,py) varvid inga skärningar
***hittas och det hela avslutas. Med optimering avstängd
***funkar allt som det ska ! 1996-02-21 J. Kjellander
*/
#ifdef WIN32
#pragma optimize("",off)
#endif

extern DBTmat *lsyspk;

/*!******************************************************/

        DBstatus GEhatch2D(
        DBLine  *lpvek[],
        short    nlin,
        DBArc   *apvek[],
        DBSeg   *aspvek[],
        short    narc,
        DBCurve *cpvek[],
        DBSeg   *cspvek[],
        short    ncur,
        DBTmat  *csyptr,
        DBHatch *xhtpek,
        DBfloat  crdvek[])

/*      Create hatch entity from closed boundary.
 *
 *      In: lpvek  = Array of ptrs to lines.
 *          nlin   = Number of lines.
 *          apvek  = Array of ptrs to arcs.
 *          aspvek = Array of ptrs to arc segments.
 *          narc   = Number of arcs.
 *          cpvek  = Array of ptrs to curves.
 *          cspvek = Array of ptrs to curve segments.
 *          ncur   = Number of curves.
 *          csyptr = Ptr to coordinate system.
 *
 *      Out: *xhtpek = The hatch.
 *            crdvek = Array of hatchlines.
 *
 *      (C)microform ab  8/8/85 R. Svedin
 *
 *      10/9/85    Nya felkoder, R. Svedin
 *      4/8/87     Ny startpunkt för linjer, J. Kjellander
 *      16/3/88    Ny startpunkt för cirklar, J. Kjellander
 *      24/7/90    Kurvor, J. Kjellander
 *      1999-05-27 Rewritten, J.Kjellander
 *      2007-09-30 3D, J.Kjellander
 *
 ******************************************************!*/

  {
     short    ii;
     DBfloat  px,py,dpx,dpy,k;
     DBfloat  t[100];
     short    ts,a,status;
     DBfloat  xangra;
     DBVector pos;

/*
***Calculate slope and perpendicular stepsize.
*/
     xangra = xhtpek->ang_xh * DGTORD;

     if ( xhtpek->dist_xh < TOL1 ) return(erpush("GE8212","geo820"));
     dpx =   xhtpek->dist_xh * SIN(xangra);
     dpy = -(xhtpek->dist_xh * COS(xangra));

     if       ( ABS(xhtpek->ang_xh) == 90.0 ) k = 1E10;
     else if ( ABS(TAN(xangra)) > 0.0001)    k = TAN(xangra);
     else                                     k = 0.001;
/*
***Välj startpunkt för generering av 1:a snittlinjen.
***Startpunkten väljs så att en snittlinje genom antingen
***denna eller nästa (åt andra hållet) säkert måste skära
***konturen. Om konturen innehåller linjer väljs 1:a linjens
***startpunkt + 0.5*(dpx,dpy) och andra gången - 0.5*(dpx,dpy).
***Denna metod infördes 4/8/87. Tidigare valdes linjens start-
***punkt+(0.005,0.005), vilket ligger på snittlinjen om denna
***lutar 45 eller -45 grader.
****16/3/88, startpunkt för cirklar på samma sätt som för linjer.
*/
     if ( narc > 0)
        {
        GEposition((DBAny *)apvek[0],(char *)aspvek[0],0.0,(gmflt)0.0,&pos);
        px = pos.x_gm + 0.5*dpx;
        py = pos.y_gm + 0.5*dpy;
        }
     else if ( nlin > 0)
        {
        px = (lpvek[0])->crd1_l.x_gm + 0.5*dpx;
        py = (lpvek[0])->crd1_l.y_gm + 0.5*dpy;
        }
     else if ( ncur > 0)
        {
        GEposition((DBAny *)cpvek[0],(char *)cspvek[0],0.0,(gmflt)0.0,&pos);
        px = pos.x_gm + 0.5*dpx;
        py = pos.y_gm + 0.5*dpy;
        }
     else return(erpush("GE8212","geo820"));
/*
***Create lines to the right of the startpoint and
***calculate intersects.
*/
     xhtpek->nlin_xh = 0;
     a  = 0;

l22:
     status = GEint_2Dlb(lpvek,nlin,apvek,aspvek,narc,cpvek,cspvek,ncur,
                         px,py,k,t,&ts);
     if ( status < 0 ) return(status);

     if ( ts < 2 ) goto l25;
/*
***Beräkna koord. och lagra.
*/
     ii = 1;

l66:
     if ( xhtpek->nlin_xh == GMXMXL ) return(erpush("GE8202","geo820"));
     crdvek[a]   = px + t[ii] / k;
     crdvek[a+1] = py + t[ii];
     crdvek[a+2] = px + t[ii+1] / k;
     crdvek[a+3] = py + t[ii+1];

     ii = ii + 2;
     a  = a + 4;
     xhtpek->nlin_xh += 1;

     if ( ii < ts) goto l66;

     px = px + dpx;
     py = py + dpy;

     goto l22 ;
/*
***Inga fler skärningar åt höger. Välj ny startpunkt och
***generera linjer åt vänster.
*/
l25:
     dpx = -dpx;
     dpy = -dpy;

     if ( narc > 0 )
        {
        px = pos.x_gm + 0.5*dpx;
        py = pos.y_gm + 0.5*dpy;
        }
     else if ( nlin > 0 )
        {
        px = (lpvek[0])->crd1_l.x_gm + 0.5*dpx;
        py = (lpvek[0])->crd1_l.y_gm + 0.5*dpy;
        }
     else
        {
        px = pos.x_gm + 0.5*dpx;
        py = pos.y_gm + 0.5*dpy;
        }
/*
***Beräkna skärning.
*/
l27:
     status = GEint_2Dlb(lpvek,nlin,apvek,aspvek,narc,cpvek,cspvek,ncur,
                         px,py,k,t,&ts);
     if ( status < 0 ) return(status);
/*
***Normalt slut.
*/
     if ( ts < 2 )
       {
       return(0);
       }
/*
***Beräkna koord. och lagra. Parametern t representerar skär-
***ningens avstånd från (px,py) i Y-led. Eftersom k aldrig
***tillåts bli=0 kan heller aldrig t bli lika med 0. Horisontella
***linjer approximeras ju med k=0.001.
*/
     ii = 1;

l67:
     if ( xhtpek->nlin_xh == GMXMXL ) return(erpush("GE8202","geo820"));
     crdvek[a]   = px + t[ii] / k;
     crdvek[a+1] = py + t[ii];
     crdvek[a+2] = px + t[ii+1] / k;
     crdvek[a+3] = py + t[ii+1];

     ii = ii + 2;
     a  = a + 4;
     xhtpek->nlin_xh += 1;

     if ( ii < ts) goto l67;

     px = px + dpx;
     py = py + dpy;

     goto l27 ;
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEarea2D(
        DBLine   *lpvek[],
        short     nlin,
        DBArc    *apvek[],
        DBSeg    *aspvek[],
        short     narc,
        DBCurve  *cpvek[],
        DBSeg    *cspvek[],
        short     ncur,
        DBfloat   dist,
        DBfloat  *area,
        DBVector *tp)

/*      Computes area and center of gravity for 2D closed
 *      boundary.
 *
 *      In: lpvek = The lines.
 *          nlin  = Number of lines.
 *          apvek = The arcs.
 *          narc  = Number of arcs.
 *          cpvek = The curves.
 *          spvek = Their segments.
 *          ncur  = Number of curves.
 *          dist  = Accuracy, distance between lines.
 *
 *      Ut: *area = Area.
 *          *tp   = Center of gravity.
 *
 *      (C)microform ab  26/7/90 J. Kjellander
 *
 *       1999-05-27 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
     short    i;
     DBfloat  px,py,dpx,dpy,k,da,lcgy;
     DBfloat  t[100];
     short    ts,status;
     bool     first;
     DBVector pos;

/*
***Steg och lutning för snittlinjer.
*/
     dpx  = dist;
     dpy  = 0.0;
     k    = 1E10;
    *area = 0.0;
/*
***Startpunkt för generering av 1:a snittlinjen.
*/
     if ( narc > 0)
        {
        GEposition((DBAny *)apvek[0],(char *)aspvek[0],
                   (gmflt)0.0,(gmflt)0.0,&pos);
        px = pos.x_gm + 0.5*dpx;
        py = pos.y_gm + 0.5*dpy;
        }
     else if ( nlin > 0)
        {
        px = (lpvek[0])->crd1_l.x_gm + 0.5*dpx;
        py = (lpvek[0])->crd1_l.y_gm + 0.5*dpy;
        }
     else if ( ncur > 0)
        {
        GEposition((DBAny *)cpvek[0],(char *)cspvek[0],
                   (gmflt)0.0,(gmflt)0.0,&pos);
        px = pos.x_gm + 0.5*dpx;
        py = pos.y_gm + 0.5*dpy;
        }
     else return(erpush("GE8212","geo824"));
/*
***Generera linjer åt höger och beräkna skärningar.
*/
     first = TRUE;
nxtlinr:
     status = GEint_2Dlb(lpvek,nlin,apvek,aspvek,narc,cpvek,cspvek,ncur,
                     px,py,k,t,&ts);
     if ( status < 0 ) return(status);
/*
***En linje har skurits och resulterat i ingen, en eller flera
***linjer. Om ingen skärning erhållits är vi klara med linjer
***åt höger.
*/
     if ( ts < 2 ) goto left;

     if ( first )
       {
       tp->x_gm = px;
       tp->y_gm = (py + t[1] + py + t[2])/2.0;
       first = FALSE;
       }

     for ( i=0; i<ts; i+=2 )
       {
/*
***Delytans storlek.
*/
       da = dist*ABS(t[i+1] - t[i+2]);
/*
***Tyngdpunktens X-koordinat. Om den nya delytan ligger till höger
***om tp skall tp.x uppdateras. Den kan ju också ligga ovanför eller
***under och då bryr vi oss inte om den.
*/
       if ( px > tp->x_gm )
          tp->x_gm = tp->x_gm + (px-tp->x_gm)*(da/(*area+da));
/*
***Tp:s Y-koordinat. Om delytan ligger över eller under tp skall
***tp.y uppdateras.
*/
       lcgy = (py + t[i+1] + py + t[i+2])/2.0;
       if ( lcgy > tp->y_gm )
         tp->y_gm = tp->y_gm + (lcgy - tp->y_gm)*(da/(*area+da));
       else if ( lcgy < tp->y_gm )
         tp->y_gm = tp->y_gm - (tp->y_gm - lcgy)*(da/(*area+da));
/*
***Totala ytan.
*/
       *area = *area + da;
       }

     px += dpx; py += dpy;
     goto nxtlinr;
/*
***Inga fler skärningar åt höger. Välj ny startpunkt och
***generera linjer åt vänster.
*/
left: 
     dpx = -dist;

     if ( narc > 0 )
        {
        px = pos.x_gm + 0.5*dpx;
        py = pos.y_gm + 0.5*dpy;
        }
     else if ( nlin > 0 )
        {
        px = (lpvek[0])->crd1_l.x_gm + 0.5*dpx;
        py = (lpvek[0])->crd1_l.y_gm + 0.5*dpy;
        }
     else
        {
        px = pos.x_gm + 0.5*dpx;
        py = pos.y_gm + 0.5*dpy;
        }
/*
***Beräkna skärning.
*/
nxtlinl: 
     status = GEint_2Dlb(lpvek,nlin,apvek,aspvek,narc,cpvek,cspvek,ncur,
                     px,py,k,t,&ts);
     if ( status < 0 ) return(status);

     if ( ts == 0 ) return(0);             /* utgång */

     if ( first )
       {
       tp->x_gm = px;
       tp->y_gm = (py + t[1] + py + t[2])/2.0;
       first = FALSE;
       }

     for ( i=0; i<ts; i+=2 )
       {
       da = dist*ABS(t[i+1] - t[i+2]);
       if ( px < tp->x_gm )
          tp->x_gm = tp->x_gm - (tp->x_gm-px)*(da/(*area+da));
       lcgy = (py + t[i+1] + py + t[i+2])/2.0;
       if ( lcgy > tp->y_gm )
         tp->y_gm = tp->y_gm + (lcgy - tp->y_gm)*(da/(*area+da));
       else if ( lcgy < tp->y_gm )
         tp->y_gm = tp->y_gm - (tp->y_gm - lcgy)*(da/(*area+da));
       *area = *area + da;
       }

     px += dpx; py += dpy;
     goto nxtlinl;
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEint_2Dlb(
        DBLine  *lpvek[],
        short    nlin,
        DBArc   *apvek[],
        DBSeg   *aspvek[],
        short    narc,
        DBCurve *cpvek[],
        DBSeg   *cspvek[],
        short    ncur,
        DBfloat  px,
        DBfloat  py,
        DBfloat  k,
        DBfloat  t[],
        short   *ts)

/*      Computes all the intersects between a line and a
 *      boundary.
 *
 *      In: lpvek  = The lines
 *          nlin   = Number of lines
 *          apvek  = The arcs
 *          aspvek = Their segments
 *          narc   = Number of arcs
 *          cpvek  = The curves
 *          cspvek = Their segments
 *          ncur   = Number of curves
 *          px     = Line start x
 *          py     = Line start y
 *          k      = TAN(line slope)
 *
 *      Out: t    = Array of intersect perametric values
 *           ts   = Number of intersects
 *
 *      (C)microform ab  8/8/85 R. Svedin
 *
 *      1998-05-04 n_udda, J.Kjellander
 *      1999-05-26 Rewritten, J.Kjellander
 *      2007-10-06 3D, J.Kjellander
 *
 ******************************************************!*/

  {
     DBfloat d,k2;
     short   n_udda,swap,i,j,ii,noint,status;
     DBfloat v,ta,uout1[INTMAX],uout2[INTMAX];
     DBLine  lin;

/*
***Init.
*/
     n_udda = 0;
     k2 = k * k;
/*
***Lines.
*/
linjer:
     ii = 0;
    *ts = 0;
     if ( nlin < 1 ) goto cirklar;

l33:
     d = (lpvek[ii])->crd2_l.x_gm - (lpvek[ii])->crd1_l.x_gm -
       ((lpvek[ii])->crd2_l.y_gm - (lpvek[ii])->crd1_l.y_gm) / k;

     if ( d == 0 ) goto l39;

     ta = (px - (lpvek[ii])->crd1_l.x_gm + ((lpvek[ii])->crd1_l.y_gm -
        py) / k) / d;

     if ( ta < 0.0 || ta > 1.0 ) goto l37;

     *ts = *ts + 1;
     t[*ts] = (lpvek[ii])->crd1_l.y_gm + ((lpvek[ii])->crd2_l.y_gm -
            (lpvek[ii])->crd1_l.y_gm) * ta - py;

l37:
     ii = ii + 1;
     if ( ii < nlin ) goto l33;
     else goto cirklar;

l39:
     if ( px - (lpvek[ii])->crd1_l.x_gm - 
        (py - (lpvek[ii])->crd1_l.y_gm) / k  == 0.0 )
        {
        px = px + 0.0005;
        py = py + 0.0005;
        goto linjer;
        }

     goto l37;
/*
***Arcs. From SVN#31 assumed to always be 3D.
*/
cirklar:
     if ( narc == 0 ) goto kurvor;
/*
***Create temporary line entity.
*/
     lin.hed_l.type = LINTYP; v = ATAN(k);
     lin.crd1_l.x_gm = px;
     lin.crd1_l.y_gm = py; 
     lin.crd1_l.z_gm = 0.0;
     lin.crd2_l.x_gm = px + COS(v);
     lin.crd2_l.y_gm = py + SIN(v);
     lin.crd2_l.z_gm = 0.0;
/*
***Calculate line/arc intersects.
*/
     for ( ii=0; ii<narc; ++ii)
       {
       noint  = -1;
       status = GE710((DBAny *)&lin,NULL,(DBAny *)apvek[ii],aspvek[ii],
                                                lsyspk,&noint,uout1,uout2);
       if ( status < 0 ) return(erpush("GE8232",""));
/*
***GE710() använder GEsort_1() för att sortera och ta bort dubletter.
***Villkoret för två lika skärningar är i GEsort_1() = TOL4. Detta är
***ett mycket litet tal som inte säkert tar bort alla dubletter.
***För att vara säkra inför vi en egen variant här med tol = 0.05.
*/
       for ( i=0; i<noint-1; ++i )
         {
         if ( ABS(uout1[i] - uout1[i+1]) < 0.05 )
           {
           for ( j=i; j<noint-1; ++j )
             {
             uout1[j] = uout1[j+1];
             }
           noint--;
           }
         }
/*
***geo710() returnerar skärningar som parametervärden relaterade till
***linjen och i intervallet 1->2 från linjens start till slut. geo820()
***förutsätter parametervärden melan 0->oändligheten lika med skärningens
***dy. Eftersom linjens längd är ett kan parametervärdena transformeras
***genom en enkel multiplikation med SIN(v).
*/
       for ( i=0; i<noint; ++i )
         {
         *ts += 1;
         t[*ts] = (uout1[i] - 1.0)*SIN(v);
         }
       }
/*
***Curves.
*/
kurvor:
     if ( ncur == 0 ) goto sort;
/*
***Create temporary line entity.
*/
     lin.hed_l.type = LINTYP; v = ATAN(k);
     lin.crd1_l.x_gm = px;
     lin.crd1_l.y_gm = py; 
     lin.crd1_l.z_gm = 0.0;
     lin.crd2_l.x_gm = px + COS(v);
     lin.crd2_l.y_gm = py + SIN(v);
     lin.crd2_l.z_gm = 0.0;
/*
***Calculate line/curve intersects.
*/
     for ( ii=0; ii<ncur; ++ii)
       {
       noint  = -1;
       status = GE710((DBAny *)&lin,NULL,(DBAny *)cpvek[ii],cspvek[ii],
                                                lsyspk,&noint,uout1,uout2);
       if ( status < 0 ) return(erpush("GE8232",""));
/*
***Remove duplicates.
*/
       for ( i=0; i<noint-1; ++i )
         {
         if ( ABS(uout1[i] - uout1[i+1]) < 0.05 )
           {
           for ( j=i; j<noint-1; ++j )
             {
             uout1[j] = uout1[j+1];
             }
           noint--;
           }
         }
/*
***Reparametrize.
*/
       for ( i=0; i<noint; ++i )
         {
         *ts += 1;
         t[*ts] = (uout1[i] - 1.0)*SIN(v);
         }
       }
/*
***I t finns nu alla *ts skärningar mellan snittlinjen
***och storhetena i konturen som parametervärden relaterade
***snittlinjen.
***Bubbel-sortera skärningarnas t-värden.
***
***Om antalet skärningar är ett ojämt tal, flytta linjen
***och prova igen tills jämt tal eller 0 erhålls. 6/2/91 J. Kjellander
***Eller tills n_udda > 100. Om det inträffar är det troligen fel
***på konturen.
*/
sort:
     if ( *ts == 0 ) return(0);

     if ( ( *ts/2.0 - floor(*ts/2.0) ) > 0.1 )
        {
        px = px + 0.005;
        py = py + 0.005;
        if ( ++n_udda > 100 )
          return(erpush("GE8312",""));
        goto linjer;
        }

l60:
     ii = 1;
     swap  = 0;

l61:
     if ( t[ii+1] < t[ii] )
        {
        d = t[ii];
        t[ii] = t[ii+1];
        t[ii+1] = d;
        swap = 1;
        }
 /*
***Om två skärningar är identiska flyttar vi skärnings-
***linjen och provar igen tills alla skärningar är olika.
*/
     if ( ABS(t[ii+1] - t[ii]) < TOL1)
        {
        px = px + 0.005;
        py = py + 0.005;
        goto linjer;
        }

     ++ii;

     if ( ii == 99 ) return(erpush("GE8222","GEint_2Dlb")); /* Felutgång */

     if ( ii < *ts) goto l61;
     if ( swap != 0) goto l60;

     return(0);
  }

/********************************************************/
