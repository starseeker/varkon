/**********************************************************************
*
*    wpcsy.c
*    =======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPdrcs();    Draw coordinate system
*    WPdlcs();    Delete coordinate system
*    WPupcs();    Uppdate coordinate system
*    WPplcs();    Create 3D polyline
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
#include "../../GE/include/GE.h"
#include "../include/WP.h"

extern int actpen;

static short drawcs(WPGWIN *gwinpt, DBCsys *csypek, DBptr la, bool draw);

#ifdef WIN32
extern bool msgrst();
#endif

static int csmode = V3_CS_NORMAL;

/*!******************************************************/

        short WPdrcs(
        DBCsys  *csypek,
        DBptr   la,
        DBint   win_id)

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
***Loopa igenom alla WPGWIN-fönster.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
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
         if ( WPnivt(gwinpt->nivtab,csypek->hed_pl.level) )
           {
/*
***Ja. Kolla att rätt färg är inställd.
*/
           if ( csypek->hed_pl.pen != actpen ) WPspen(csypek->hed_pl.pen);
#ifdef UNIX
           if ( csmode == V3_CS_ACTIVE )
             {
             if ( WPgrst("varkon.act_csys_pen",buf)  &&
             sscanf(buf,"%d",&ival) == 1 ) WPspen((short)ival);
             }
#endif

#ifdef WIN32
           if ( csmode == V3_CS_ACTIVE )
             {
             if ( msgrst("ACT_CSYS_PEN",buf)  &&
             sscanf(buf,"%d",&ival) == 1 ) WPspen((short)ival);
             }
#endif

/*
***Sen är det bara att rita.
*/
           drawcs(gwinpt,csypek,la,TRUE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short WPdlcs(
        DBCsys  *csypek,
        DBptr   la,
        DBint   win_id)

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
***Loopa igenom alla WPGWIN-fönster.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
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
***Detta är särskilt viktigt om vi ska aktivera/deaktivera.
***Det gamla ligger då i DF.
*/
         if ( WPfobj(gwinpt,la,CSYTYP,&typ) )
           {
           WPdobj(gwinpt,FALSE);
           WProbj(gwinpt);
           }
/*
***Om det nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda långsamt.
*/
         else
           {
           if ( !WPnivt(gwinpt->nivtab,csypek->hed_pl.level)  ||
                               csypek->hed_pl.blank) return(0);
           drawcs(gwinpt,csypek,la,FALSE);         
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/********************************************************/

        short   WPupcs(
        DBCsys *csypek,
        DBptr   la,
        int     mode,
        DBint   win_id)

/*      Redisplays a coordinate system in normal or active
 *      mode in one or all WPGWIN's.
 *
 *      In: csypek = C ptr to csys
 *          la     = DB ptr to csys
 *          mode   = V3_CS_NORMAL or V3_CS_ACTIVE
 *          win_id = ID of WPGWIN to update or GWIN_ALL
 *
 *      (C)2008-03-15 J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Set mode for WPplcs().
*/
   csmode = mode;
/*
***Erase 
*/
   WPdlcs(csypek,la,win_id);
/*
***Display.
*/
   WPdrcs(csypek,la,win_id);
/*
***Reset original mode.
*/
   csmode = V3_CS_NORMAL;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        static short drawcs(
        WPGWIN *gwinpt,
        DBCsys  *csypek,
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
   double lgdx,lgdy,lgd;
   short  status;
   int    k;

/*
***Axlarnas längd 20% av modellfönstrets bredd eller höjd,
***vi tar det största.
*/
   lgdx = gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin;
   lgdy = gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin; 

   if ( lgdx > lgdy ) lgd = lgdx;
   else               lgd = lgdy;
/*
***Skapa grafisk representation, dvs. polylinje.
*/
   k = -1;
   if ( (status=WPplcs(csypek,lgd,csmode,&k,x,y,z,a)) < 0 ) return(status);
/*
***Project on current view of window.
*/
   WPpply(gwinpt,k,x,y,z);
/*
***Klipp polylinjen. Om den är synlig (helt eller delvis ),
***rita den.
*/
   if ( WPcply(&gwinpt->vy.modwin,(short)-1,&k,x,y,a) )
     {
     if ( draw  &&  csypek->hed_pl.hit )
       {
       if ( WPsply(gwinpt,k,x,y,a,la,CSYTYP) ) WPdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else WPdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short   WPplcs(
        DBCsys *csypek,
        double  size,
        int     mode,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    a[])

/*      Skapar 3D-polylinje för koordinatsystem.
 *
 *      In: 
 *          csypek => Pekare till csy-structure
 *          size   => X-axelns längd
 *          mode   => V3_CS_NORMAL/V3_CS_ACTIVE
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
    short    status;
    int      k;
    double   lgd,lgdz;
    DBTmat   invmat;
    DBVector pl,pb;

/*
***Initiering.
*/
   k = *n + 1;
   lgd = size;
   if ( mode == V3_CS_ACTIVE ) lgd *= 0.3;
   else                        lgd *= 0.15;
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
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = 0;

   pl.x_gm = lgd;
   pl.y_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = VISIBLE;

   pl.x_gm =  0.9*lgd;
   pl.y_gm = -0.05*lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = VISIBLE;

   if ( mode == V3_CS_ACTIVE )
     {
     pl.x_gm = 0.9*lgd;
     pl.y_gm = 0.05*lgd;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[k]   = pb.x_gm;
     y[k]   = pb.y_gm;
     z[k]   = pb.z_gm;
     a[k++] = VISIBLE;
     }
/*
***Själva Y-axeln.
*/
   pl.x_gm = 0.0;
   pl.y_gm = lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = 0;

   if ( mode == V3_CS_NORMAL)
     {
     pl.x_gm = 0.0;
     pl.y_gm = 0.59*lgd;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[k]   = pb.x_gm;
     y[k]   = pb.y_gm;
     z[k]   = pb.z_gm;
     a[k++] = VISIBLE;

     pl.x_gm = 0.0;
     pl.y_gm = 0.51*lgd;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[k]   = pb.x_gm;
     y[k]   = pb.y_gm;
     z[k]   = pb.z_gm;
     a[k++] = 0;
     }

   pl.x_gm = 0.0;
   pl.y_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = VISIBLE;
/*
***Själva X-axeln.
*/
   if ( mode == V3_CS_NORMAL)
     {
     pl.x_gm = 0.51*lgd;
     pl.y_gm = 0.0;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[k]   = pb.x_gm;
     y[k]   = pb.y_gm;
     z[k]   = pb.z_gm;
     a[k++] = VISIBLE;

     pl.x_gm = 0.59*lgd;
     pl.y_gm = 0.0;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[k]   = pb.x_gm;
     y[k]   = pb.y_gm;
     z[k]   = pb.z_gm;
     a[k++] = 0;
     }

   pl.x_gm = lgd;
   pl.y_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = VISIBLE;
/*
***Z-axel med pilspets.
*/
   lgdz = 0.3*lgd;
   pl.x_gm = 0.0;
   pl.y_gm = 0.0;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = 0;

   pl.z_gm = lgdz;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = VISIBLE;

   pl.y_gm = 0.05*lgdz;
   pl.z_gm = 0.9*lgdz;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = 0;

   pl.y_gm = 0.0;
   pl.z_gm = lgdz;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = VISIBLE;

   pl.y_gm = -0.05*lgdz;
   pl.z_gm =  0.9*lgdz;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = VISIBLE;

   if ( mode == V3_CS_ACTIVE )
     {
     pl.y_gm = 0.05*lgdz;
     pl.z_gm = 0.9*lgdz;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[k]   = pb.x_gm;
     y[k]   = pb.y_gm;
     z[k]   = pb.z_gm;
     a[k++] = VISIBLE;
     }
/*
***En pilspets på Y-axeln.
*/
   pl.z_gm =  0.0;
   pl.x_gm = -0.05*lgd;
   pl.y_gm =  0.9*lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = 0;

   pl.x_gm = 0.0;
   pl.y_gm = lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k++] = VISIBLE;

   pl.x_gm = 0.05*lgd;
   pl.y_gm = 0.9*lgd;
   GEtfpos_to_local(&pl,&invmat,&pb);
   x[k]   = pb.x_gm;
   y[k]   = pb.y_gm;
   z[k]   = pb.z_gm;
   a[k] = VISIBLE;

   if ( mode == V3_CS_ACTIVE )
     {
   ++k;
     pl.x_gm = -0.05*lgd;
     pl.y_gm =  0.9*lgd;
     GEtfpos_to_local(&pl,&invmat,&pb);
     x[k] = pb.x_gm;
     y[k] = pb.y_gm;
     z[k] = pb.z_gm;
     a[k] = VISIBLE;
     }
/*
***Slut.
*/
  *n = k;

   return(0);
  }

/********************************************************/
