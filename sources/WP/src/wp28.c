/**********************************************************************
*
*    wp28.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpdrcs();    Draw coordinate system
*    wpdlcs();    Delete coordinate system
*    wpupcs();    Uppdate coordinate system
*    wpplcs();    Create viewspecific polyline
*    wpmpcs();    Create 3D polyline
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
#include "../../GE/include/GE.h"
#include "../include/WP.h"

extern short    actpen;

static short drawcs(WPGWIN *gwinpt, GMCSY *csypek, DBTmat *csymat,
                    DBptr la, bool draw);

#ifdef WIN32
extern bool msgrst();
#endif

static int csmode = V3_CS_NORMAL;

/*!******************************************************/

        short wpdrcs(
        GMCSY  *csypek,
        DBTmat *csymat,
        DBptr   la,
        v2int   win_id)

/*      Ritar ett koordinatsystem.
 *
 *      In: csypek => Pekare till koordinatsystem-post.
 *          csymat => Pekare till matrisen.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 1/2/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int     i,ival;
   char    buf[80];
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om det är blankat är det enkelt.
*/
   if ( csypek->hed_pl.blank) return(0);
/*
***Loopa igenom alla WPGWIN-fönster utom 1:a.
*/
   for ( i=1; i<WTABSIZ; ++i )
     {
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skall vi rita i detta fönster ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Ja, ligger koordinatsystemet på en nivå som är tänd i detta fönster ?
*/
         if ( wpnivt(gwinpt,csypek->hed_pl.level) )
           {
/*
***Ja. Kolla att rätt färg är inställd.
*/
           if ( csypek->hed_pl.pen != actpen ) wpspen(csypek->hed_pl.pen);
#ifdef V3_X11
           if ( csmode == V3_CS_ACTIVE )
             {
             if ( wpgrst("varkon.act_csys_pen",buf)  &&
             sscanf(buf,"%d",&ival) == 1 ) wpspen((short)ival);
             }
#endif

#ifdef WIN32
           if ( csmode == V3_CS_ACTIVE )
             {
             if ( msgrst("ACT_CSYS_PEN",buf)  &&
             sscanf(buf,"%d",&ival) == 1 ) wpspen((short)ival);
             }
#endif

/*
***Sen är det bara att rita.
*/
           drawcs(gwinpt,csypek,csymat,la,TRUE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpdlcs(
        GMCSY  *csypek,
        DBTmat *csymat,
        DBptr   la,
        v2int   win_id)

/*      Suddar ett koordinatsystem.
 *
 *      In: csypek => Pekare till koordinatsystem-post.
 *          csymat => Pekare till matrisen.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 1/2/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   DBetype typ;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Loopa igenom alla WPGWIN-fönster utom 1:a.
*/
   for ( i=1; i<WTABSIZ; ++i )
     {
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skall vi sudda i detta fönster ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Ja. Om den finns i DF kan vi sudda snabbt.
*/
         if ( wpfobj(gwinpt,la,CSYTYP,&typ) )
           {
           wpdobj(gwinpt,FALSE);
           wprobj(gwinpt);
           }
/*
***Om det nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda långsamt.
*/
         else
           {
           if ( !wpnivt(gwinpt,csypek->hed_pl.level)  ||
                               csypek->hed_pl.blank) return(0);
           drawcs(gwinpt,csypek,csymat,la,FALSE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpupcs(
        GMCSY  *csypek,
        DBTmat *pmat,
        DBptr   la,
        int     mode,
        v2int   win_id)

/*      Uppdaterar ett koordinatsystem.
 *
 *      In: csypek = Pekare till GM-post
 *          pmat   = Transformationsmatris
 *          la     = Planets GM-adress.
 *          mode   = V3_CS_NORMAL eller V3_CS_ACTIVE
 *
 *      Ut: Inget.
 *
 *      FV:     0 => Ok.
 *
 *      (C)microform ab 1997-03-11 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Sätt mode.
*/
   csmode = mode;
/*
***Sudda.
*/
   wpdlcs(csypek,pmat,la,win_id);
/*
***Rita igen.
*/
   wpdrcs(csypek,pmat,la,win_id);
/*
***Återställ mode.
*/
   csmode = V3_CS_NORMAL;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        static short drawcs(
        WPGWIN *gwinpt,
        GMCSY  *csypek,
        DBTmat *csymat,
        DBptr   la,
        bool    draw)

/*      Ritar/suddar ett koordinatsystem i ett visst fönster.
 *      Vid ritning lagras objektet samtidigt i DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          csypek => Pekare till koordinatsystem-post.
 *          csymat => Pekare till matrisen.
 *          la     => GM-adress.
 *          draw   => TRUE = Rita, FALSE = Sudda
 *
 *      Ut:  Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/2/95 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   short  status;
   int    k;

/*
***Skapa grafisk representation, dvs. polylinje.
*/
   k = -1;
   if ( (status=wpplcs(gwinpt,csypek,csymat,&k,x,y,z,a)) < 0 ) return(status);
/*
***Klipp polylinjen. Om den är synlig (helt eller delvis ),
***rita den.
*/
   if ( wpcply(gwinpt,(short)-1,&k,x,y,a) )
     {
     if ( draw  &&  csypek->hed_pl.hit )
       {
       if ( wpsply(gwinpt,k,x,y,a,la,CSYTYP) ) wpdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else wpdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/***********************************************************/
/*!******************************************************/

        short wpplcs(
        WPGWIN *gwinpt,
        GMCSY  *csypek,
        DBTmat *csymat,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    a[])

/*      Bygger ett koordinatsystem i form av en polylinje.
 *
 *      In: gwinpt => Pekare till fönster.
 *          csypek => Pekare till csy-structure
 *          csymat => Pekare till matrisen
 *          n+1    => Offset till polylinjens startposition
 *
 *      Ut: *n     => Offset till polylinjens slutposition
 *           x,y,a => Polylinjens koordinater och status
 *
 *      FV: 0
 *
 *      (C)microform ab 1/2/95 J. Kjellander
 *
 *      1997-03-11 csmod, J.Kjellander
 *
 ******************************************************!*/

  {
    int    k;
    short  status;
    gmflt  lgdx,lgdy,lgd,lgdz;
    DBTmat invmat;
    DBVector  pb,pl;
    WPGRPT pt;

/*
***Initiering.
*/
   k = *n + 1;
/*
***Invertera matrisen en gång för alla så går transformationerna
***fortare sen.
*/
   if ( (status=GEtform_inv(csymat,&invmat)) < 0 ) return(status);
/*
***Axlarnas längd 20% av modellfönstrets bredd eller höjd,
***vi tar det största.
*/
   lgdx = gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin;
   lgdy = gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin; 

   if ( lgdx > lgdy ) lgd = lgdx;
   else               lgd = lgdy;

   if ( csmode == V3_CS_ACTIVE ) lgd *= 0.3;
   else                          lgd *= 0.15;
/*
***En pilspets på X-axeln.
*/
   pl.x_gm = 0.9*lgd;
   pl.y_gm = 0.05*lgd;
   pl.z_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   wptrpo(gwinpt,&pb,&pt);
   x[k]   = pt.x;
   y[k]   = pt.y;
   a[k++] = 0;

   pl.x_gm = lgd;
   pl.y_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   wptrpo(gwinpt,&pb,&pt);
   x[k]   = pt.x;
   y[k]   = pt.y;
   a[k++] = VISIBLE;

   pl.x_gm =  0.9*lgd;
   pl.y_gm = -0.05*lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   wptrpo(gwinpt,&pb,&pt);
   x[k]   = pt.x;
   y[k]   = pt.y;
   a[k++] = VISIBLE;

   if ( csmode == V3_CS_ACTIVE )
     {
     pl.x_gm = 0.9*lgd;
     pl.y_gm = 0.05*lgd;
     GEtfpos_to_local(&pl,&invmat,&pb);
     wptrpo(gwinpt,&pb,&pt);
     x[k]   = pt.x;
     y[k]   = pt.y;
     a[k++] = VISIBLE;
     }
/*
***Y-axeln.
*/
   pl.x_gm = 0.0;
   pl.y_gm = lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   wptrpo(gwinpt,&pb,&pt);
   x[k]   = pt.x;
   y[k]   = pt.y;
   a[k++] = 0;

   if ( csmode == V3_CS_NORMAL)
    {
    pl.x_gm = 0.0;
    pl.y_gm = 0.59*lgd;
    GEtfpos_to_local(&pl,&invmat,&pb);
    wptrpo(gwinpt,&pb,&pt);
    x[k]   = pt.x;
    y[k]   = pt.y;
    a[k++] = VISIBLE;

    pl.x_gm = 0.0;
    pl.y_gm = 0.51*lgd;
    GEtfpos_to_local(&pl,&invmat,&pb);
    wptrpo(gwinpt,&pb,&pt);
    x[k]   = pt.x;
    y[k]   = pt.y;
    a[k++] = 0;
    }

   pl.x_gm = 0.0;
   pl.y_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   wptrpo(gwinpt,&pb,&pt); 
   x[k]   = pt.x;
   y[k]   = pt.y;
   a[k++] = VISIBLE;
/*
***X-axeln.
*/
   if ( csmode == V3_CS_NORMAL)
    {
    pl.x_gm = 0.51*lgd;
    pl.y_gm = 0.0;
    GEtfpos_to_local(&pl,&invmat,&pb);
    wptrpo(gwinpt,&pb,&pt);
    x[k]   = pt.x;
    y[k]   = pt.y;
    a[k++] = VISIBLE;

    pl.x_gm = 0.59*lgd;
    pl.y_gm = 0.0;
    GEtfpos_to_local(&pl,&invmat,&pb);
    wptrpo(gwinpt,&pb,&pt);
    x[k]   = pt.x;
    y[k]   = pt.y;
    a[k++] = 0;
    }

   pl.x_gm = lgd;
   pl.y_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   wptrpo(gwinpt,&pb,&pt); 
   x[k]   = pt.x;
   y[k]   = pt.y;
   a[k++] = VISIBLE;
/*
***Z-axel med pilspets ritas bara om det är en sned vy
***eller ett snett koordinatsystem.
*/
   if ( gwinpt->vy.vy_3D  ||  csymat->g11 != 1.0  ||
                              csymat->g22 != 1.0  ||
                              csymat->g33 != 1.0 )
     {
     lgdz = 0.3*lgd;
     pl.x_gm = 0.0;
     pl.y_gm = 0.0;
     GEtfpos_to_local(&pl,&invmat,&pb);
     wptrpo(gwinpt,&pb,&pt); 
     x[k]   = pt.x;
     y[k]   = pt.y;
     a[k++] = 0;

     pl.z_gm = lgdz;
     GEtfpos_to_local(&pl,&invmat,&pb);
     wptrpo(gwinpt,&pb,&pt); 
     x[k]   = pt.x;
     y[k]   = pt.y;
     a[k++] = VISIBLE;

     pl.y_gm = 0.05*lgdz;
     pl.z_gm = 0.9*lgdz;
     GEtfpos_to_local(&pl,&invmat,&pb);
     wptrpo(gwinpt,&pb,&pt);
     x[k]   = pt.x;
     y[k]   = pt.y;
     a[k++] = 0;

     pl.y_gm = 0.0;
     pl.z_gm = lgdz;
     GEtfpos_to_local(&pl,&invmat,&pb);
     wptrpo(gwinpt,&pb,&pt);
     x[k]   = pt.x;
     y[k]   = pt.y;
     a[k++] = VISIBLE;

     pl.y_gm = -0.05*lgdz;
     pl.z_gm =  0.9*lgdz;
     GEtfpos_to_local(&pl,&invmat,&pb);
     wptrpo(gwinpt,&pb,&pt);
     x[k]   = pt.x;
     y[k]   = pt.y;
     a[k++] = VISIBLE;

     if ( csmode == V3_CS_ACTIVE )
       {
       pl.y_gm = 0.05*lgdz;
       pl.z_gm = 0.9*lgdz;
       GEtfpos_to_local(&pl,&invmat,&pb);
       wptrpo(gwinpt,&pb,&pt);
       x[k]   = pt.x;
       y[k]   = pt.y;
       a[k++] = VISIBLE;
       }
     }
/*
***En pilspets på Y-axeln.
*/
   pl.z_gm =  0.0;
   pl.x_gm = -0.05*lgd;
   pl.y_gm =  0.9*lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   wptrpo(gwinpt,&pb,&pt);
   x[k]   = pt.x;
   y[k]   = pt.y;
   a[k++] = 0;

   pl.x_gm = 0.0;
   pl.y_gm = lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   wptrpo(gwinpt,&pb,&pt);
   x[k]   = pt.x;
   y[k]   = pt.y;
   a[k++] = VISIBLE;

   pl.x_gm = 0.05*lgd;
   pl.y_gm = 0.9*lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   wptrpo(gwinpt,&pb,&pt);
   x[k]   = pt.x;
   y[k]   = pt.y;
   a[k] = VISIBLE;

   if ( csmode == V3_CS_ACTIVE )
     {
   ++k;
     pl.x_gm = -0.05*lgd;
     pl.y_gm =  0.9*lgd;
     GEtfpos_to_local(&pl,&invmat,&pb);
     wptrpo(gwinpt,&pb,&pt);
     x[k] = pt.x;
     y[k] = pt.y;
     a[k] = VISIBLE;
     }
/*
***Slut.
*/
  *n = k;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpmpcs(
        GMCSY  *csypek,
        double  size,
        int     font,
        int    *npts,
        double  x[],
        double  y[],
        double  z[],
        char    a[])

/*      Skapar 3D-polylinje för koordinatsystem.
 *
 *      In: 
 *          csypek => Pekare till csy-structure
 *          size   => X-axelns längd
 *          font   => V3_CS_NORMAL/V3_CS_ACTIVE
 *          n+1    => Offset till polylinjens startposition
 *
 *      Ut: *npts  => Antal punkter i polylinjen
 *           x,y,a => Polylinjens koordinater och status
 *
 *      FV: 0
 *
 *      (C)microform ab 1998-10-04, J. Kjellander
 *
 ******************************************************!*/

  {
    int    n;
    short  status;
    double lgd,lgdz;
    DBTmat invmat;
    DBVector  pl,pb;

/*
***Initiering.
*/
   n = *npts = 0;
   lgd = size;
/*
***Invertera matrisen en gång för alla så går transformationerna
***fortare sen.
*/
   if ( (status=GEtform_inv(&csypek->mat_pl,&invmat)) < 0 ) return(status);
/*
***Pilspetsen på X-axeln.
*/
   pl.x_gm = 0.9*lgd;
   pl.y_gm = 0.05*lgd;
   pl.z_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = 0;

   pl.x_gm = lgd;
   pl.y_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = VISIBLE;

   pl.x_gm =  0.9*lgd;
   pl.y_gm = -0.05*lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = VISIBLE;

   if ( font == V3_CS_ACTIVE )
     {
     pl.x_gm = 0.9*lgd;
     pl.y_gm = 0.05*lgd;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[n]   = pb.x_gm;
     y[n]   = pb.y_gm;
     z[n]   = pb.z_gm;
     a[n++] = VISIBLE;
     }
/*
***Själva Y-axeln.
*/
   pl.x_gm = 0.0;
   pl.y_gm = lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = 0;

   if ( font == V3_CS_NORMAL)
     {
     pl.x_gm = 0.0;
     pl.y_gm = 0.59*lgd;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[n]   = pb.x_gm;
     y[n]   = pb.y_gm;
     z[n]   = pb.z_gm;
     a[n++] = VISIBLE;

     pl.x_gm = 0.0;
     pl.y_gm = 0.51*lgd;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[n]   = pb.x_gm;
     y[n]   = pb.y_gm;
     z[n]   = pb.z_gm;
     a[n++] = 0;
     }

   pl.x_gm = 0.0;
   pl.y_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = VISIBLE;
/*
***Själva X-axeln.
*/
   if ( font == V3_CS_NORMAL)
     {
     pl.x_gm = 0.51*lgd;
     pl.y_gm = 0.0;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[n]   = pb.x_gm;
     y[n]   = pb.y_gm;
     z[n]   = pb.z_gm;
     a[n++] = VISIBLE;

     pl.x_gm = 0.59*lgd;
     pl.y_gm = 0.0;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[n]   = pb.x_gm;
     y[n]   = pb.y_gm;
     z[n]   = pb.z_gm;
     a[n++] = 0;
     }

   pl.x_gm = lgd;
   pl.y_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = VISIBLE;
/*
***Z-axel med pilspets.
*/
   lgdz = 0.3*lgd;
   pl.x_gm = 0.0;
   pl.y_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = 0;

   pl.z_gm = lgdz;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = VISIBLE;

   pl.y_gm = 0.05*lgdz;
   pl.z_gm = 0.9*lgdz;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = 0;

   pl.y_gm = 0.0;
   pl.z_gm = lgdz;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = VISIBLE;

   pl.y_gm = -0.05*lgdz;
   pl.z_gm =  0.9*lgdz;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = VISIBLE;

   if ( font == V3_CS_ACTIVE )
     {
     pl.y_gm = 0.05*lgdz;
     pl.z_gm = 0.9*lgdz;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[n]   = pb.x_gm;
     y[n]   = pb.y_gm;
     z[n]   = pb.z_gm;
     a[n++] = VISIBLE;
     }
/*
***En pilspets på Y-axeln.
*/
   pl.z_gm =  0.0;
   pl.x_gm = -0.05*lgd;
   pl.y_gm =  0.9*lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = 0;

   pl.x_gm = 0.0;
   pl.y_gm = lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n++] = VISIBLE;

   pl.x_gm = 0.05*lgd;
   pl.y_gm = 0.9*lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[n]   = pb.x_gm;
   y[n]   = pb.y_gm;
   z[n]   = pb.z_gm;
   a[n] = VISIBLE;

   if ( font == V3_CS_ACTIVE )
     {
   ++n;
     pl.x_gm = -0.05*lgd;
     pl.y_gm =  0.9*lgd;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[n] = pb.x_gm;
     y[n] = pb.y_gm;
     z[n] = pb.z_gm;
     a[n] = VISIBLE;
     }
/*
***Slut.
*/
  *npts = n;

   return(0);
  }

/********************************************************/
