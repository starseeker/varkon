/*!*****************************************************
*
*    exview.c
*    ========
*
*    EXcrvp();     Interface-routine for CRE_VIEW by position
*    EXcrvc();     Interface-routine for CRE_VIEW by coord.sys.
*    EXacvi();     Interface-routine for ACT_VIEW
*    EXscvi();     Interface-routine for SCL_VIEW
*    EXcevi();     Interface-routine for CEN_VIEW
*    EXhdvi();     Interface-routine for HIDE_VIEW
*    EXprvi();     Interface-routine for PERP_VIEW
*    EXervi();     Interface-routine for ERASE_VIEW
*    EXrpvi();     Interface-routine for REP_VIEW
*    EXcavi();     Interface-routine for CACC_VIEW
*    EXplwi();     Interface-routine for PLOT_WIN
*
*    This file is part of the VARKON Execute Library.
*    URL:  http://www.varkon.com
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
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../WP/include/WP.h"
#include "../include/EX.h"
#include <string.h>

extern DBTmat *lsyspk;
extern DBTmat  lklsys,lklsyi;

/*!******************************************************/

        short     EXcrvp(
        char      name[],
        DBVector *campos)

/*      Creates/updates a view using a camera position and
 *      looking at Basic (0,0,0) with Y up. CRE_VIEW() in MBS.
 *      Used in evview.c
 *
 *      In: name   => View name.
 *          campos => Camera position.
 *
 *      Error: EX1672 = View_tab full.
 *
 *      (C)microform ab 22/7/86 R. Svedin
 *
 *      10/10/86 Tagit bort param. för vyfönster, R. Svedin
 *      18/10/86 gpgwin(), J. Kjellander
 *      24/10/91 Bytt namn till EXcrvp(), J. Kjellander
 *      29/9/92  vytypp, J. Kjellander
 *      23/1-95  Multifönster, J. Kjellander
 *      2007-01-01 removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
   WPVIEW  view;

/*
***View name.
*/
   strcpy(view.name,name);
/*
***Calculate the view transformation matrix.
*/
   WPcampos_to_matrix(campos,&view.matrix);
/*
***Save the new view in WP.
*/
   if ( WPcreate_view(&view,VIEW_3D_ONLY) < 0 ) return(erpush("EX1672",name));
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXcrvc(
        char  name[],
        DBId *idvek)

/*      Creates/updates a view using a coordinate system.
 *      CRE_VIEW() in MBS.
 *      Used in evview.c
 *
 *      In: name  => View name.
 *          idvek => ID of csys.
 *
 *      Error: EX1672 = view_tab full.
 *             EX1402 = Entity does not exist.
 *
 *      (C)microform ab 29/9/92 J. Kjellander
 *
 *      23/1-95  Multifönster, J. Kjellander
 *      2006-12-31 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;
    DBCsys  csy;
    DBTmat  mat;
    WPVIEW  view;

/*
***Get csys from DB.
*/
    if ( DBget_pointer('I',idvek,&la,&typ) < 0  ||  typ != CSYTYP )
        return(erpush("EX1402","EXcrvc"));
    DBread_csys(&csy,&mat,la);
/*
***View name.
*/
   strcpy(view.name,name);
/*
***Copy matrix from csys.
*/
   view.matrix.k11 = mat.g11;
   view.matrix.k12 = mat.g12;
   view.matrix.k13 = mat.g13;

   view.matrix.k21 = mat.g21;
   view.matrix.k22 = mat.g22;
   view.matrix.k23 = mat.g23;

   view.matrix.k31 = mat.g31;
   view.matrix.k32 = mat.g32;
   view.matrix.k33 = mat.g33;
/*
***Save the new view in WP.
*/
   if ( WPcreate_view(&view,VIEW_3D_ONLY) < 0 ) return(erpush("EX1672",name));
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXacvi(
        char  name[],
        DBint win_id)

/*      Makes a view active in a window. ACT_VIEW() in MBS.
 *
 *      In: name   => View name.
 *          win_id => ID of WPGWIN.
 *
 *      Error: EX1682 = View does not exist.
 *             EX2162 = Window not WPGWIN/WPRWIN.
 *             EX2172 = Window does not exist.
 *
 *      (C)2007-02-13 J.Kjellander
 *
 ******************************************************!*/

  {
   char    errbuf[V3STRLEN];
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;

/*
***Get a C-ptr to the window and activate.
*/
   if ( wpwtab[win_id].ptr != NULL )
     {
     switch ( wpwtab[win_id].typ )
       {
       case TYP_GWIN:  
       gwinpt = (WPGWIN *)wpwtab[win_id].ptr;
       if ( WPactivate_view(name,gwinpt,NULL,TYP_GWIN) < 0 )
          return(erpush("EX1682",name));
       break;

       case TYP_RWIN:  
       rwinpt = (WPRWIN *)wpwtab[win_id].ptr;
       if ( WPactivate_view(name,NULL,rwinpt,TYP_RWIN) < 0 )
          return(erpush("EX1682",name));
       break;

       default:
       sprintf(errbuf,"%d",win_id);
       return(erpush("EX2162",errbuf));
       break;
       }
     return(0);
     }
   else
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("EX2172",errbuf));
     }
  }

/********************************************************/
/*!******************************************************/

        short   EXscvi(
        char    name[],
        DBfloat scl)

/*      Interface-rutin för SCL_VIEW().
 *      Måste göras om !!!!!!! Man kan inte skala en namngiven
 *      vy i vytab eftersom vytab inte har några fönsterstorlekar.
 *      Man kan skala den vy som är aktiv i ett visst fönster.
 *      MBS SCL_VIEW(vynamn,skala) borde alltså ersättas med
 *      SCL_VIEW(fönster_id,skala);
 *
 *      OBS-OBS-OBS: Tills vidare ändrar vi skalan i GWIN_MAIN
 *      utan att uppdatera vytab.
 *
 *      In: name => Vyns namn.
 *          scl  => Ny skala.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1682 = Vy med detta namn finns ej.
 *                EX1692 = Kan ej minska skalan till begärt värde
 *                EX1702 = Kan ej skapa fönster.
 *
 *      (C)2006-12-31, J.Kjellander
 *
 ******************************************************!*/

  {
   double  mdx,mdy,xmin,xmax,ymin,ymax;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Check for invalid scale factor.
*/
   if ( scl <= 1e-10 ) return(erpush("EX1702",""));
/*
***Get a ptr to GWIN_MAIN. To be changed in the future so
***that any window can be used.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   gwinpt = (WPGWIN *)winptr->ptr;
/*
***Size of current window.
*/
   mdx   =  gwinpt->geo.psiz_x *
           (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   mdy   =  gwinpt->geo.psiz_y *
           (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
/*
***Vi har nu en ny skalfaktor och kan beräkna ett nytt modellfönster.
*/
   xmin =  gwinpt->vy.modwin.xmin + 
          (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/2.0 -
                     mdx/2.0/scl;
   xmax =  gwinpt->vy.modwin.xmin +
          (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/2.0 + 
                     mdx/2.0/scl;
   ymin =  gwinpt->vy.modwin.ymin +
          (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/2.0 -
                     mdy/2.0/scl;
   ymax =  gwinpt->vy.modwin.ymin +
          (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/2.0 +
                     mdy/2.0/scl;
/*
***Save previous and activate new scale.
*/
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVIEW));

   gwinpt->vy.modwin.xmin = xmin;
   gwinpt->vy.modwin.xmax = xmax;
   gwinpt->vy.modwin.ymin = ymin;
   gwinpt->vy.modwin.ymax = ymax;
   WPnrgw(gwinpt);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short   EXcevi(
        char    name[],
        DBfloat x,
        DBfloat y)

/*      Interface-rutin för CEN_VIEW. Sätter ett nytt centrum.
 *      OBS !!!!!!! Samma som SCL_VIEW(), se ovan.
 *
 *      In: name  => Vyns namn.
 *          x     => X-koord för nytt centrum.
 *          y     => Y-koord för nytt centrum.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1682 = Vy med detta namn finns ej.
 *                EX1692 = Kan ej minska skalan till begärt värde
 *                EX1702 = Kan ej skapa fönster.
 *
 *      (C)2006-12-31 J.Kjellander
 *
 ******************************************************!*/

  {
    double  xmin,ymin,xmax,ymax,
            new_xmin,new_ymin,new_xmax,new_ymax;
    WPWIN  *winptr;
    WPGWIN *gwinpt;

/*
***Get a ptr to GWIN_MAIN. To be changed in the future so
***that any window can be used.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   gwinpt = (WPGWIN *)winptr->ptr;
/*
***The current window limits.
*/
   xmin = gwinpt->vy.modwin.xmin;
   xmax = gwinpt->vy.modwin.xmax;
   ymin = gwinpt->vy.modwin.ymin;
   ymax = gwinpt->vy.modwin.ymax;
/*
***Calculate new limits.
*/
   new_xmin = x - (xmax - xmin)/2.0; 
   new_xmax = x + (xmax - xmin)/2.0; 
   new_ymin = y - (ymax - ymin)/2.0; 
   new_ymax = y + (ymax - ymin)/2.0; 
/*
***Save previous and activate new limits.
*/
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVIEW));

   gwinpt->vy.modwin.xmin = new_xmin;
   gwinpt->vy.modwin.xmax = new_xmax;
   gwinpt->vy.modwin.ymin = new_ymin;
   gwinpt->vy.modwin.ymax = new_ymax;
   WPnrgw(gwinpt);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXhdvi(
        char      name[],
        bool      flag1,
        bool      flag2,
        FILE     *pfil,
        DBVector *origo)

/*      Interface routine for HIDE_VIEW(). 
 *      To be changed to support any window.
 *
 *      In: name  => Vyns namn
 *          flag1 => TRUE = Rita på skärm
 *          flag2 => TRUE = Rita till fil
 *          pfil  => Filpekare
 *          origo => Pekare till nollpunkt eller NULL
 *
 *      Ut: Inget.
 *
 *      FV:      0 = Ok.
 *          AVBRYT = <CTRL>c i WPhide().
 *
 *      Felkoder: EX1682 = Vy med detta namn finns ej.
 *
 *      (C)microform ab 21/3/89 J. Kjellander
 *
 *      29/5/91  Origo, J. Kjellander
 *      2006-12-28 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    DBVector nollp;
    WPWIN   *winptr;
    WPGWIN  *gwinpt;
    WPGRPT   projpos;

/*
***Get a ptr to the main graphics window.
*/
    winptr = WPwgwp((wpw_id)GWIN_MAIN);
    gwinpt = (WPGWIN *)winptr->ptr;
/*
***Transformera origo till basic och sedan till aktiv vy och
***beräkna avstånd till vy:ns nedre vänstra hörn. Om origo = NULL
***ta nedre vänstra hörnet på aktiv vy.
*/
    if ( origo != NULL )
      {
      if ( lsyspk != NULL ) GEtfpos_to_local(origo,&lklsyi,origo);

      WPppos((WPGWIN *)winptr->ptr,origo,&projpos);
      origo->x_gm = projpos.x;
      origo->y_gm = projpos.y;
      origo->z_gm = 0.0;

      nollp.x_gm = origo->x_gm - gwinpt->vy.modwin.xmin;
      nollp.y_gm = origo->y_gm - gwinpt->vy.modwin.ymin;
      nollp.z_gm = 0.0;
      }
    else nollp.x_gm = nollp.y_gm = nollp.z_gm = 0.0;
/*
***Make hide in the main window.
*/
    status = WPhide((WPGWIN *)winptr->ptr,flag1,flag2,pfil,&nollp);
/*
***The end.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short   EXprvi(
        char    name[],
        DBfloat dist)

/*      Interface-rutin för PERP_VIEW. Ändrar perspektiv-
 *      avstånd.
 *
 *      In: name  => Vyns namn.
 *          dist  => Nytt avstånd. 0 = Parallellprojektion.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1682 = Vy med detta namn finns ej.
 *                EX1922 = Avstånd < 0.
 *
 *      (C)2006-12-31 J.Kjellander
 *
 ******************************************************!*/

 {
    WPWIN  *winptr;
    WPGWIN *gwinpt;

/*
***Check distance for validity.
*/
    if ( dist < 0.0 ) return(erpush("EX1922",""));
/*
***Get a ptr to the main graphics window.
*/
    winptr = WPwgwp((wpw_id)GWIN_MAIN);
    gwinpt = (WPGWIN *)winptr->ptr;
/*
***Save previous and activate new perspective distance.
*/
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVIEW));

   gwinpt->vy.pdist = dist;
/*
***The end.
*/
    return(0);
 }

/********************************************************/
/*!******************************************************/

        short EXervi(DBint win_id)

/*      Interface-rutin för ERASE_VIEW(). Suddar fönster.
 *
 *      In: win_id => Fönsterid.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 30/6/86 R. Svedin
 *
 *      20/1-95  Multifönster, J. Kjellander
 *      2006-12-29 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Erase requested graphic window.
*/
    WPergw(win_id);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXrpvi(
        DBint autoz,
        DBint win_id)

/*      Interface-rutin för REP_VIEW(). Genererar en ny bild.
 *
 *      In: autoz  => Autozoom = 1 annars 0.
 *          win_id => Fönster att rita om.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 30/6/86 R. Svedin
 *
 *      7/10/86  Bytt till repagm, R. Svedin
 *      1/1/95   Multifönster, J. Kjellander
 *      1996-12-19 WIN32, J.Kjellander
 *      2006-12-28 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
    short status;

/*
***X-Windows.
*/
#ifdef UNIX
    if ( autoz == 1 )
      {
      status = WPmaut(win_id);
      return(status);
      }
    else
      {
      WPrepaint_GWIN(win_id);
      return(0);
      }
#endif

/*
***Microsoft Windows.
*/
#ifdef WIN32
    if ( autoz == 1 )
      {
      status = (short)msmaut(win_id);
      return(status);
      }
    else
      {
      msrepa(win_id);
      return(0);
      }
#endif
  }

/********************************************************/
/*!******************************************************/

        short EXcavi(DBfloat newcn)

/*      Interface-rutin för CACC_VIEW(). Ändra kurvnogranhet.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder:  EX1712 = Felaktig kurvnogranhet.
 *
 *      (C)microform ab 4/8/86 R. Svedin
 *
 ******************************************************!*/

  {
/*
*** Ändra kurvnogranheten.
*/
    if (newcn < 0.01 || newcn > 100.0 ) return(erpush("EX1712",""));

    WPset_cacc(newcn);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXplwi(
        DBint     grw_id,
        DBVector *p1,
        DBVector *p2,
        char     *filnam,
        DBVector *p0)

/*      Interface routine for PLOT_WIN().
 *      Creates a GKS plotfile of the contents ow grw_id
 *      clipped to the borders of p1,p2 and translated
 *      with -p0.
 *
 *      In: gtw_id = ID of graphical window.
 *          p1,p2  = Bounding rectangle.
 *          filnam = Ptr to output filename.
 *          p0     = Plot origin.
 *
 *      Error:  EX2172 = Window %s does not exist.
 *              EX2162 = Window %s is of illegal type.
 *              EX1742 = Can't create plotfile %s, OS error.
 *
 *      (C)2007-04-08 J. Kjellander
 *
 ******************************************************!*/

  {
    char     errbuf[V3STRLEN];
    DBVector origo;
    VYWIN    plotwin;
    FILE    *filpek;
    WPWIN   *winptr;
    WPGWIN  *gwinpt;

/*
***Get a ptr to the graphics window. Currently, only
***WPGWIN's are allowed.
*/
    winptr = WPwgwp((wpw_id)grw_id);
    if ( winptr == NULL )
      {
      sprintf(errbuf,"%d",grw_id);
      return(erpush("EX2172",errbuf));
      }

    if ( winptr->typ != TYP_GWIN )
      {
      sprintf(errbuf,"%d",grw_id);
      return(erpush("EX2162",errbuf));
      }

    gwinpt = (WPGWIN *)winptr->ptr;
/*
***Set up the plot area.
*/
    plotwin.xmin = p1->x_gm;
    plotwin.ymin = p1->y_gm;
    plotwin.xmax = p2->x_gm;
    plotwin.ymax = p2->y_gm;
/*
***Cretate GKS_meta_file or DXF_file.
*/
    if ( (filpek=fopen(filnam,"w+")) == NULL )
      {
      erpush("EX1742",filnam);
      goto error;
      }

    if ( strcmp(filnam+strlen(filnam)-4,DXFEXT) == 0 )
      {
      origo.x_gm = p0->x_gm;
      origo.y_gm = p0->y_gm;
      WPdxf_out(gwinpt,filpek,&plotwin,&origo);
      }
    else
      {
      origo.x_gm = p0->x_gm - p1->x_gm;
      origo.y_gm = p0->y_gm - p1->y_gm;
      WPmkpf(gwinpt,filpek,&plotwin,&origo);
      }

    fclose(filpek);
    goto exit;
/*
***Fel.
*/
error:
    errmes();
/*
***The end.
*/
exit:
    return(0);
  }

/********************************************************/
