/**********************************************************************
*
*    wpDF.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPgtlarw();  Single entity selection in WPRWIN
*    WPgtla();    Single entity selection in WPGWIN
*    WPgmla();    Selects all entities in WPRWIN rectangle
*    WPrwms();    Selects all entities in WPGWIN rectangle
*    WPgmlw();    WPgmla() with rubberband rectangle
*
*    WPsply();    Store polyline in displayfile
*
*    WPfobj();    Search for specific entity in displayfile
*    WPdobj();    Display/erase current entity
*    WProbj();    Delete entity
*
*    WPhgen();    Highlight entity in one or all windows
*    WPerhg();    Erase all highligt markers
*
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
#include <math.h>
#include <GL/glu.h>

#define DF_ERASED  -1      /* Erased DF-object */

extern int actpen;

/********************************************************/

        DBptr   WPgtlarw(
        WPRWIN *rwinpt,
        DBetype typmsk,
        int     ix,
        int     iy)

/*      Returns DB pointer of entity selected in a WPRWIN.
 *
 *      In: rwinpt = C ptr to WPRWIN.
 *          typmsk = Typemask
 *          ix,iy  = Pixel coordinates relative to low left.
 *
 *      Return: >= 0  = Valid DB pointer.
 *                -1  = No entity selected.
 *
 *      (C)2007-07-25 J.Kjellander
 *
 ******************************************************!*/

  {
#define BUFSIZE    512   /* Select buffer size */
#define MAX_PM_SIZE 100  /* Max size of pick matrix */
   
   GLuint  *hlptr,hitlist[BUFSIZE];
   int      i,n_hits,n_names,pm_size;
   float    zmax,zmin,zmin_all;
   DBptr    la;
   DBHeader hdr;

/*
***Activate the RC of this window.
*/
   glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
/*
***Tell GL where we want the result and how big it can be.
*/
   glSelectBuffer(BUFSIZ,hitlist);
/*
***Start with a small pick matrix. If no objects are found,
***increase the size and try again until objects are found
***or the pick matrix has reached its maximum size.
*/
  pm_size = 1;
  n_hits = 0;

start:
  do
    {
    glRenderMode(GL_SELECT);
    WPmodl_all(rwinpt);
    WPeodls(rwinpt,ix,iy,pm_size,pm_size);
    n_hits = glRenderMode(GL_RENDER);
    pm_size += 10.0;
    if ( n_hits > 0 ) break;
    } while ( pm_size < MAX_PM_SIZE);
/*
***We now have 0, 1 or more objects in the hitlist.
***If no hit, return -1.
*/
   if ( n_hits == 0 ) return(-1);
   else
     {
/*
***Else, process the hitlist.
*/
     hlptr    = &hitlist[0];
     zmin_all = 1E+15;
     la       = DBNULL;

     for ( i=0; i<n_hits; ++i )
       {
       n_names = *hlptr;  /* Number of names for hit i */
     ++hlptr;

       zmin = (float)*hlptr/0x7fffffff;
     ++hlptr;

       zmax = (float)*hlptr/0x7fffffff;
     ++hlptr;
/*
***Find the hit closest to the eye. For each hit that
***is closer than the previous, check entity type againts
***the type mask. 
*/
       if ( zmax <= zmin_all )
         {
         DBread_header(&hdr,*hlptr);
         if ( (hdr.type & typmsk) > 0 )
           {
           zmin_all = zmax;
           la = *hlptr;
           }
         }
     ++hlptr;
       }
     }
/*
***If we now have a hit, return it !
*/
   if ( la > 0 ) return(la);
/*
***If not and the pick matrix still can grow larger,
***try again with a larger pick matrix.
*/
   else if ( pm_size < MAX_PM_SIZE ) goto start;
/*
***If this doesn't help, return -1.
*/
   else          return(-1);
  }

/******************************************************!*/
/*!******************************************************/

        DBptr    WPgtla(
        DBint    win_id,
        DBetype  typmsk,
        short    ix,
        short    iy,
        DBetype *typ,
        bool    *ends,
        bool    *right)

/*      Returnerar GM-adress f�r storhet n�rmast
 *      f�nster-positionen ix, iy och med typ som
 *      st�mmer med typmsk.
 *
 *      In: win_id => F�nster att g�ra s�kning i.
 *          typmsk => Mask f�r ev. begr�nsad s�kning
 *          ix,iy  => Koordinat i sk�rmkoordinater
 *          typ    => Pekare till DBetype-variabel.
 *
 *      Ut: *typ   => Typ f�r den funna storheten.
 *                    Odefinierad om ingen storhet
 *                    hittats.
 *          *ends  => Sk�rmpos. ligger n�rmast obj. slut
 *          *right => Sk�rmpos. ligger p� obj. h�gra sida.
 *
 *
 *      FV: >= 0  => GM-adress f�r n�rmsta storhet.
 *            -1  => Ingen storhet hittat.
 *
 *      (C)microform ab 10/1-95 J. Kjellander
 *
 *      2006-12-08 Removed gpgtla(), J.Kjellander
 *      2007-02-14 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    DBint   i,n,j;
    DBint   imin,nmin,ilim,dist,mdist;
    DBint   t1=0,t2=0;
    DBint   ixt,iyt,idx,idy;
    char    a='\0';
    DBfloat tt;
    DFPOST *dfi,*dfn;
    WPWIN  *winptr;
    WPGWIN *gwinpt;
    WPRWIN *rwinpt;

/*
***Init.
*/
    mdist = 2000000000;
    imin = -1;   /* Pekare till n�rmsta objekt */
    nmin = 0;    /* Pekare till n�rmsta vektor */
/*
***Which window ?
*/
    if ( (winptr=WPwgwp((wpw_id)win_id)) != NULL )
      {
      if      ( winptr->typ == TYP_GWIN ) gwinpt = (WPGWIN *)winptr->ptr;
      else if ( winptr->typ == TYP_RWIN )
        {
        rwinpt = (WPRWIN *)winptr->ptr;
        return(WPgtlarw(rwinpt,typmsk,ix,iy));
         }
      else return(-1);
      }
    else return(-1);
/*
***Does a DF exist ?
*/
    if ( gwinpt->df_adr == NULL ) return(-1);
/*
***The DF search loop starts here.
*/
    for ( i=0; i<gwinpt->df_ptr;  i += 3 + dfi->hdr.nvec)
       {
       dfi = gwinpt->df_adr + i;
       if (((dfi->hdr.type & typmsk) > 0) && ((dfi+1)->la != DF_ERASED))
         {
/*
***Object type = point.
*/
         if (dfi->hdr.nvec == 0 )
           { 
           ixt = (DBint)(ix - (dfi+2)->vec.x);
           iyt = (DBint)(iy - (dfi+2)->vec.y);

           if ((dist = ixt*ixt + iyt*iyt) < mdist)
             {
             mdist = dist;
             imin = i;
             a = (dfi+2)->vec.a;
             }
           }
/*
***Object type = polyline.
*/
         else
           {
           ilim = i + 2 + dfi->hdr.nvec;
           for ( n=i + 2; n<ilim; n++ )
              {
              dfn = gwinpt->df_adr + n;
              idx = (dfn+1)->vec.x - dfn->vec.x;
              idy = (dfn+1)->vec.y - dfn->vec.y;
              ixt = ix - dfn->vec.x;
              iyt = iy - dfn->vec.y;
              j = 0;
              if ( idx != 0  ||  idy != 0 )
                {
                if ( (t1=idx*ixt + idy*iyt) > 0 ) 
                  {                                     /* t >= 0 */
                  if ( t1 >= (t2 = idx*idx + idy*idy) )
                    {
                    ixt -= idx;                         /* t >= 1 */
                    iyt -= idy;
                    j = 1;
                    }
                  else
                    {
                    ixt -= (idx*t1)/t2;                 /* 0 < t < 1 */
                    iyt -= (idy*t1)/t2;
                    j = 2;
                    }
                  }
                }
/*
***Are we closest now ?
*/
              if ( (dist=ixt*ixt + iyt*iyt) < mdist )
                {
                if ( ((dfn+1)->vec.a & VISIBLE) == VISIBLE )
                  {
                  mdist = dist;
                  imin = i;
                  nmin = n;
                  if (j == 0) a = dfn->vec.a;             /* t = 0 */
                  else if (j == 1) a = (dfn+1)->vec.a;    /* t = 1 */
                  else if (2*t1 < t2) a = dfn->vec.a;     /* t < 0.5 */
                  else a = (dfn+1)->vec.a;                /* t >= 0.5 */
                  }
                }
             }
          }
       }
    }
/*
***Le final.
*/
    if ( imin != -1 )
      {
      gwinpt->df_cur = imin;           /* Make the object current */
      dfi = gwinpt->df_adr + imin;
      *typ = dfi->hdr.type;
/*
***For polylines, calculate end and side.
*/
      if ( dfi->hdr.nvec != 0 )
        {
        *ends = ((a & ENDSIDE) != 0);
        *right = TRUE;

        dfn = gwinpt->df_adr + nmin;
        idx = (dfn+1)->vec.y - dfn->vec.y;
        idy = dfn->vec.x - (dfn+1)->vec.x;
        ixt = ix - dfn->vec.x;
        iyt = iy - dfn->vec.y;
        tt = idx*idx + idy*idy;

        if ( tt > 0.0 )
          {
          tt = SQRT(tt);
          tt = idx*ixt/tt + idy*iyt/tt;
          if ( tt < 0.0 ) *right = FALSE;
          }
        }
      return((dfi+1)->la);
      }
/*
***Nothing found.
*/
    else return(-1);
  }
     
/********************************************************/
/*!******************************************************/

        short   WPgmla(
        DBint   win_id,
        short   ix1,
        short   iy1,
        short   ix2,
        short   iy2,
        short   mode,
        bool    hl,
        short  *nla,
        DBetype typvek[],
        DBptr   lavek[])

/*      Returns DBptrs to all objects in a rectangular
 *      area of a graphical window.
 *
 *      In: ix1,iy1  = Rectangle corner 1
 *          ix2,iy2  = Rectangle corner 2
 *          mode     = 0 = All inside
 *                     1 = All inside or crossing
 *                     2 = All outside
 *                     3 = All outside or crossing
 *          hl       = TRUE => Highligt
 *          *nla     = Max number of objects to return
 *          typvek[0]= Typemask
 *          win_id   = Window to use for search
 *
 *      Out: *nla     = Number of output entities
 *           *typvek  = Entity types
 *           *lavek   = Entity DBptrs
 *
 *      Return:  0 = Ok
 *              -1 = Illegal window
 *
 *      (C) microform ab 13/1/95 J. Kjellander
 *
 *      1997-04-10 Bug, J.Kjellander
 *      2006-12-08 Removed gpgtla(), J.Kjellander
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

 {
    DBint   i,j,k,n;
    short   maxobj,nv,x,y,nhit,nin,nut;
    DBetype orgtyp;
    bool    inside,outside,hit;
    DFPOST *df;
    WPWIN  *winptr;
    WPGWIN *gwinpt;
    WPRWIN *rwinpt;
    VYWIN   tmpwin;
    WPGRPT  pt1,pt2;

/*
***What kind of window ? Start with WPRWIN.
***Do a GL selection and highligt the selected entities.
*/
    if ( (winptr=WPwgwp((wpw_id)win_id)) != NULL  &&  winptr->typ == TYP_RWIN )
      {
      rwinpt = (WPRWIN *)winptr->ptr;
      WPrwms(rwinpt,lavek,&n,typvek,(ix1+ix2)/2,(iy1+iy2)/2,ix2-ix1,iy2-iy1);
      if ( hl )
        {
        WPmodl_highlight(rwinpt,n,lavek);
        WPsodl_all(rwinpt);
        }
     *nla = n;
      return(0);
      }
    else if ( (winptr=WPwgwp((wpw_id)win_id)) != NULL  &&  winptr->typ == TYP_GWIN )
      {
      gwinpt = (WPGWIN *)winptr->ptr;
      }
    else return(-1);
/*
***It's a WPGWIN.
*/
    tmpwin.xmin = ix1;
    tmpwin.ymin = iy1;
    tmpwin.xmax = ix2;
    tmpwin.ymax = iy2;


    orgtyp = typvek[0];
    nhit = 0;
    maxobj = *nla;
/*
***Search through the displayfile.
*/
    df = gwinpt->df_adr;
    i  = 0;

    while ( i < gwinpt->df_ptr )
      {
/*
***Find next not erased entity of right type.
*/
      if ( (df+i+1)->la != DF_ERASED  &&  ((df+i)->hdr.type & orgtyp) > 0 )
        {
        nv = (df+i)->hdr.nvec;
/*
***Point.
*/
        if ( nv == 0 )
          {
          x = (df+i+2)->vec.x;  y = (df+i+2)->vec.y;
          inside = ( (x > ix1) && (x < ix2) && 
                     (y > iy1) && (y < iy2) );
          outside  = !inside;
          }
/*
***Polyline.
*/
        else
          {
          j = i+2;
          k = j+nv;
          nin = nut = 0;
          for ( ; j<k; ++j )
            {
            if ( ((df+j+1)->vec.a & VISIBLE) == VISIBLE )
              {
              pt1.x = (df+j)->vec.x;     pt1.y = (df+j)->vec.y;
              pt2.x = (df+j+1)->vec.x; pt2.y = (df+j+1)->vec.y;

              switch ( WPclin(&tmpwin,&pt1,&pt2) )
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
***Is it a hit ?
*/
        hit = FALSE;
        switch ( mode)
          {
          case 0: if ( inside  &&  !outside ) hit = TRUE; break;
          case 1: if ( inside ) hit = TRUE; break;
          case 2: if ( outside  &&  !inside ) hit = TRUE; break;
          case 3: if ( outside ) hit = TRUE; break;
          }
/*
***If so, save DBptr and highligt.
*/
        if ( hit )
          {
          if ( nhit == maxobj ) goto end;
          lavek[nhit] = (df+i+1)->la;
          typvek[nhit] = (df+i)->hdr.type;
          if ( hl ) WPhgen(GWIN_ALL,lavek[nhit],TRUE);
          nhit++;
          }
        }
/*
***Next object in DF.
*/
      i += 3 + (df+i)->hdr.nvec;
      }
/*
***The end.
*/
end:
     *nla = nhit;

     return(0);
 }

/********************************************************/
/*!******************************************************/

        short    WPgmlw(
        DBptr    lavek[],
        int     *idant,
        DBetype  typvek[],
        short    hitmod)

/*      Interactive multiple selection using WPgtsw()
 *      and WPgmla(). Used by MBS routine IDENT_2().
 *
 *      In: *idant     = Max number of ID's.
 *           typvek[0] = Typemask.
 *           hitmod    = 0 => All completely inside
 *                       1 => All completely or partially inside
 *                       2 => All completely outide
 *                       3 => All completely or partially outside
 *
 *      Out: *idmat  = ID's.
 *           *idant  = Actual number of ID's selected.
 *           *typvek = Types.
 *
 *      Return: 0, REJECT, GOMAIN from WPgtsw().
 *
 *      (C)microform ab 1998-03-19 J. Kjellander
 *
 *      1998-04-29 Bug nref -> short, J.Kjellander
 *      2007-08-01 1.19 J.Kjellander
 *
 ******************************************************!*/

  {
    short   status,nref;
    int     ix1,ix2,iy1,iy2,tmp;
    int     xc,yc,xwidth,yheight;
    bool    ends;
    bool    right;
    wpw_id  grw_id;
    WPWIN  *winptr;
    WPGWIN *gwinpt;
    WPRWIN *rwinpt;

/*
***Init.
*/
   nref   = *idant;
  *idant  = 0;
/*
***Get window coordinates.
*/
   status = WPgtsw(&grw_id,&ix1,&iy1,&ix2,&iy2,hitmod,FALSE);
   if ( status == REJECT  ||  status == GOMAIN ) return(status);
/*
***Sort.
*/
   if ( ix2 < ix1 ) { tmp = ix1; ix1 = ix2; ix2 = tmp; }
   if ( iy2 < iy1 ) { tmp = iy1; iy1 = iy2; iy2 = tmp; } 
/*
***Translate window ID to C pointer. This function only
***supports WPGWIN's.
*/
   if ( (winptr=WPwgwp((wpw_id)grw_id)) != NULL  &&
         winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;
     }
   else if ( (winptr=WPwgwp((wpw_id)grw_id)) != NULL  &&
         winptr->typ == TYP_RWIN )
     {

     rwinpt = (WPRWIN *)winptr->ptr;

     xc = (ix1+ix2)/2;
     yc = (iy1+iy2)/2;
     xwidth  = ix2-ix1;
     yheight = iy2-iy1;
/*
***Call multiple selection function, process the hits record, 
***returns the arrays of DB pointers and types of selected 
***entities respectively.
*/
     WPrwms(rwinpt,lavek,idant,typvek,xc,yc,xwidth,yheight);
     return(0);
  }

/*
***If p1=p2 (status=-1) the user released the mouse button 
***without moving the mouse. In that case we use single selection
***ie. WPgtla(). If p1 != p2 we use multiple selection ie. WPgmla().
*/
   if ( status == -1 )
     {
     lavek[0] = WPgtla(gwinpt->id.w_id,typvek[0],(short)ix1,(short)iy1,
                                    &typvek[0],&ends,&right);
     if ( lavek[0] > 0 ) *idant = 1;
     else                *idant = 0;
     }
   else
     {
     WPgmla(gwinpt->id.w_id,(short)ix1,(short)iy1,(short)ix2,(short)iy2,
                                    hitmod,FALSE,&nref,typvek,lavek);
    *idant = nref;
     }
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

        short    WPrwms(
        WPRWIN  *rwinpt,
        DBptr    lavek[],
        int     *idant,
        DBetype  typvek[],
        int      xc,
        int      yc,
        int      xwidth,
        int      yheight)

/*  Returns DB pointers and types of entities selected in a WPRWIN.
 *
 *  In:  rwinpt = C ptr to WPRWIN.
 *       ix,iy  = center of pick matrix relative to low left.
 *       xwidth = width of picking matrix.
 *       yheight= height of picking matrix.
 *
 *  Out: lavek  = Valid DB pointers of selected entities vector.
 *       idant  = number of selected entities.
 *       typvek = selected entities type vector.
 *
 *  Return:  0  = Success multiple selection
 *          -1  = No entity selected.
 *
 *      (C)2007-08-05 Mohamed Rahayem.
 *
 **************************************************************/

  {
   GLuint   *hlptr,hitlist[IGMAXID];
   int      i;
   DBHeader hdr;

/*
***Init.
*/
   *idant = 0;
/*
***Activate the RC of this window.
*/
   glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
/*
***Tell GL where we want the result and how big it can be.
*/
   glSelectBuffer(IGMAXID,hitlist);
/*
***Enter selection mode.
*/
   glRenderMode(GL_SELECT);
/*
***Make OpenGL displaylist for all entities.
*/
    WPmodl_all(rwinpt);
/*
***Executes OpenGL DisplayList 1 for Selection
*/
    WPeodls(rwinpt,xc,yc,xwidth,yheight);
/*
***Exit selection mode &Get No of hits.
*/
  *idant = glRenderMode(GL_RENDER);
/*
***We now have 0, 1 or more objects in the hitlist.
***If no hit, return -1.
*/
   if ( *idant == 0 ) return(-1);  /* No entity selected */
   else
     {
/*
***Else, process the hitlist.
*/
       hlptr = &hitlist[0];
       for(i = 0; i<*idant; ++i)
         {
       ++hlptr;
       ++hlptr;
       ++hlptr;
/*
***Get DB pointer for each entity.
*/
         lavek[i] = *hlptr;
/*
***Get Type value of each entity.
*/
         DBread_header(&hdr,*hlptr);
         typvek[i] = hdr.type;
       ++hlptr;
         }
/*
***The end.
*/
     return(0);
     }
  }
/********************************************************/
/********************************************************/

   bool WPsply(
        WPGWIN  *gwinpt,
        int      k,
        double   x[],
        double   y[],
        char     a[],
        DBptr    la,
        DBetype  typ)

/*      Lagrar grapac-polyline i DF.
 *
 *      In: k   => Antal vektorer
 *          x   => X-koordinater
 *          y   => Y-koordinater
 *          a   => Vektorstatus
 *          la  => Storhetens adress i GM.
 *          typ => Typ av storhet.
 *
 *      FV: TRUE = Vektorn fick plats i displayfilen.
 *
 *      Felkod: GP0112 - Kan ej malloc()/realloc()
 *
 *      (C) microform ab 8/1-95 J. Kjellander
 *
 *       2004-07-22 Mesh, J.Kjellander, �rebro university
 *
 *********************************************************/
 {
   DBint   i;
   DFPOST *df;

/*
***Se till att plats finns.
*/
loop:
   if ( gwinpt->df_ptr + k + 3 >= gwinpt->df_all )
     {
     if ( gwinpt->df_adr == NULL )
       {
       if ( (gwinpt->df_adr=(DFPOST *)v3mall((unsigned)
                               (PLYMXV*sizeof(DFPOST)),"WPsply")) == NULL )
         {
         erpush("GP0112","");
         errmes();
         return(FALSE);
         }
       else
         {
         gwinpt->df_all = PLYMXV;
         goto loop;
         }
       }
     else
       {
       if ( (gwinpt->df_adr=(DFPOST *)v3rall((char *)gwinpt->df_adr,
               (unsigned)((gwinpt->df_all+PLYMXV)*sizeof(DFPOST)),
                                                    "WPsply")) == NULL )
         {
         erpush("GP0112","");
         errmes();
         return(FALSE);
         }
       else
         {
         gwinpt->df_all += PLYMXV;
         goto loop;
         }
       }
     }
/*
***Lagra objektet. F�rst headern.
*/
   gwinpt->df_cur = gwinpt->df_ptr + 1;
   df = gwinpt->df_adr + gwinpt->df_cur;

   df->hdr.type = typ;             /* Objektets typ */
   df->hdr.nvec = k;               /* Antal vektorer */
   df->hdr.hili = 0;               /* Ingen highlight */
   df++;
   df->la       = la;              /* Adress i GM */
/*
***Vektorerna.
*/
   for ( i=0; i<=k; i++ )
     {
     df++;
     df->vec.x = (short)(gwinpt->vy.k1x + gwinpt->vy.k2x*x[i]);
     df->vec.y = (short)(gwinpt->vy.k1y + gwinpt->vy.k2y*y[i]);
     df->vec.a = a[i];
     }
/*
***Uppdatera DF-pekaren.
*/
   gwinpt->df_ptr += k + 3;

   return(TRUE);
 }
/********************************************************/
/*!******************************************************/

        bool WPfobj(
        WPGWIN  *gwinpt,
        DBptr    la,
        DBetype  typmsk,
        DBetype *typ)

/*      Kolla om en storhet med viss la finns i ett
 *      f�nsters DF.
 *
 *      In: gwinpt => Pekare till WPGWIN-f�nster.
 *          la     => Storhetens adress i GM.
 *          typmsk => Typmask f�r ev. begr�nsad s�kning.
 *
 *      Ut: *typ            Den funna storhetens typ.
 *          *gwinpt->df_cur Om storheten finns.
 *
 *      FV:  TRUE  = Storheten finns i DF:en.
 *           FALSE = Storheten finns ej i DF:en.
 *
 *      (C) microform ab 10/1-95  J. Kjellander
 *
 ******************************************************!*/

 {
   DBint   i;
   DFPOST *df;

/*
***Om ingen DF finns �r det inget tvivel om saken.
*/
   if ( gwinpt->df_adr == NULL ) return(FALSE);
/*
***Kanske �r det aktuellt objekt som s�kes ?
*/
   if ( gwinpt->df_cur >= 0 )
     {
     df = gwinpt->df_adr + gwinpt->df_cur;
     if ( ((df+1)->la == la ) && ((df->hdr.type & typmsk) > 0) )
       {
      *typ = df->hdr.type;
       return(TRUE);
       }
     }
/*
***Inte, d� �r det b�st att s�ka igenom hela displayfilen.
***F�r vi tr�ff g�r vi objektet till nytt aktuellt objekt.
*/
   i = 0;

   while ( i < gwinpt->df_ptr )
     {
     df = gwinpt->df_adr + i;
     if ( ((df+1)->la == la) && ((df->hdr.type & typmsk) > 0) )
       {
       gwinpt->df_cur = i;
      *typ = df->hdr.type;
       return(TRUE);
       }
     i += 3 + df->hdr.nvec;
     }

   return(FALSE);
 }

/********************************************************/
/********************************************************/

   bool WPdobj(
        WPGWIN *gwinpt,
        bool    s)

/* 
 *      Ritar (suddar) aktuellt (dfcur) grafiskt objekt.
 *
 *      In: gwinpt => Pekare till f�nster att rita i.
 *          s      => Rita/Sudda.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE.
 *
 *      (C) microform ab 8/1-95 J. Kjellander
 *
 *      1996-12-17 WIN32, J.Kjellander
 *
 *********************************************************/

 {
   int     i,np,nvec;
   DFPOST *df;

#ifdef UNIX
   XPoint  ip[PLYMXV];
#endif
#ifdef WIN32
   POINT   ip[PLYMXV];
#endif
/*
***Initiering.
*/
   np   = 0;
   df   = gwinpt->df_adr + gwinpt->df_cur;
   nvec = df->hdr.nvec;
   df  += 2;
/*
***Rita.
*/
   if ( s )
     {
     for ( i=0; i<=nvec; i++ )
       {
/*
***T�nd f�rflyttning.
*/
       if ( (df->vec.a & VISIBLE) == VISIBLE ) 
         {
         ip[np].x = df->vec.x;
         ip[np].y = gwinpt->geo.dy - df->vec.y;
         df++, np++;
         }
/*
***Sl�ckt.
*/
        else
         {
         if ( np > 0 )
           {
#ifdef UNIX
           XDrawLines(xdisp,gwinpt->id.x_id,
                            gwinpt->win_gc,ip,np,CoordModeOrigin);
           XDrawLines(xdisp,gwinpt->savmap,
                            gwinpt->win_gc,ip,np,CoordModeOrigin);
#endif
#ifdef WIN32
           Polyline(gwinpt->dc,ip,np);
           Polyline(gwinpt->bmdc,ip,np);
#endif
           np = 0;
           }
         ip[np].x = df->vec.x;
         ip[np].y = gwinpt->geo.dy - df->vec.y;
         df++, np++;
         }
       }
/*
***Dom sista t�nda.
*/
     if ( np > 0 )
       {
#ifdef UNIX
       XDrawLines(xdisp,gwinpt->id.x_id,gwinpt->win_gc,ip,np,CoordModeOrigin);
       XDrawLines(xdisp,gwinpt->savmap,gwinpt->win_gc,ip,np,CoordModeOrigin);
#endif
#ifdef WIN32
       Polyline(gwinpt->dc,ip,np);
       Polyline(gwinpt->bmdc,ip,np);
#endif
       np = 0;
       }
     }
/*
***Sudda. H�r g�r vi return direkt utan att t�mma buffrar
***eftersom annars buffrar kommer att t�mmas tv� g�nger varav
***ena g�ngen tom.
*/
   else
     {
     if ( actpen != 0 ) WPspen(0);
     return(WPdobj(gwinpt,TRUE));
     }
/*
***T�m outputbufferten.
*/
#ifdef UNIX
   XFlush(xdisp);
#endif

   return(TRUE);
 }
/********************************************************/
/********************************************************/

        bool WProbj(
        WPGWIN *gwinpt)

/*      Suddar aktuellt grafiskt objekt ur en DF.
 *
 *      In: gwinpt => Pekare till WPGWIN-f�nster.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE.
 *
 *      (C) microform ab 11/1-95 J. Kjellander
 *
 *********************************************************/

 {
   DFPOST *df;

/*
***Om f�nstrets DF har ett aktuellt objekt markerar
***vi det som suddat. Om det �r highlightat suddar vi
***f�rst highlighten fr�n sk�rmen.
*/
   if ( gwinpt->df_cur >= 0 )
     {
     df = gwinpt->df_adr + gwinpt->df_cur;
     if ( df->hdr.hili == TRUE ) WPhgen(gwinpt->id.w_id,(df+1)->la,FALSE);
     (df+1)->la = DF_ERASED;
     gwinpt->df_cur = -1;
     }

   return(TRUE);
 }

/********************************************************/
/********************************************************/

        void  WPhgen(
        DBint win_id,
        DBptr la,
        bool  draw)

/*      Highlights/Unhighlights one entity in one or all
 *      WPGWIN's and WPRWIN's.
 *
 *      In: win_id => ID for WPGWIN/WPRWIN or GWIN_ALL.
 *          la     => Entity adress in DB (or NULL for WPRWIN-erase).
 *          draw   => TRUE = show, FALSE = erase
 *
 *      (C) microform ab 10/1-95 J. Kjellander
 *
 *      1996-12-17 WIN32, J.Kjellander
 *      2007-06-16 1.19, J.Kjellander
 *
 ******************************************************!*/

 {
   DBint   nv;
   short   ix=0,iy=0;
   DBetype typ;
   wpw_id  i;
   DFPOST *df;
   DBptr   la_arr[1];
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;

#ifdef UNIX
   XPoint  ip[5];
#endif
#ifdef WIN32
   POINT   ip[5];
#endif

/*
***Loop through all WPGWIN's/WPRWIN's.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( ( win_id == GWIN_ALL  ||  win_id == i )  &&  ( (winptr=WPwgwp(i)) != NULL ) )
       {
       if ( winptr->typ == TYP_GWIN )
         {
/*
***A WPGWIN found.
*/
         gwinpt = (WPGWIN *)winptr->ptr;
/*
***Find the object in the DF.
*/
         if ( WPfobj(gwinpt,la,ALLTYP,&typ) )
           {
/*
***Fast method if draw == TRUE and object is already highlighted
***or the other way around.
*/
           df = gwinpt->df_adr + gwinpt->df_cur;
           if ( df->hdr.hili == draw ) return;
/*
***Find the vector that carries the highlight mark.
*/
           nv = df->hdr.nvec;

           if ( nv == 0 )
             {
             ix = (df+2)->vec.x;
             iy = (df+2)->vec.y;
             }
           else
             {
             nv /= 2;
             df += 2 + nv;

             while ( nv >= 0 )
               {
               if ( ((df+1)->vec.a & VISIBLE) == VISIBLE )
                 {
                 ix = ((df+1)->vec.x + df->vec.x)/2;
                 iy = ((df+1)->vec.y + df->vec.y)/2;
                 break;
                 }
               nv--;
               df++;
               }
             }
/*
***Mark action in DF.
*/
           if ( nv >= 0 )
             {
             df = gwinpt->df_adr + gwinpt->df_cur;
             df->hdr.hili = draw;
/*
***Draw/erase.
*/
             if (  draw  &&  actpen != WP_ENTHG ) WPspen(WP_ENTHG);
             if ( !draw  &&  actpen != 0 ) WPspen(0);

             iy = gwinpt->geo.dy - iy;

             ip[0].x = ix + 4; ip[0].y = iy;
             ip[1].x = ix;     ip[1].y = iy + 4;
             ip[2].x = ix - 4; ip[2].y = iy;
             ip[3].x = ix;     ip[3].y = iy - 4;
             ip[4].x = ix + 4; ip[4].y = iy;

#ifdef UNIX
             XDrawLines(xdisp,gwinpt->id.x_id,
                              gwinpt->win_gc,ip,5,CoordModeOrigin);
             XDrawLines(xdisp,gwinpt->savmap,
                              gwinpt->win_gc,ip,5,CoordModeOrigin);
             XFlush(xdisp);
#endif
#ifdef WIN32
             Polyline(gwinpt->dc,ip,5);
             Polyline(gwinpt->bmdc,ip,5);
#endif
             }
           }
         }
/*
***A WPRWIN found. Erase for single entity is not yet
***implemented, only erase all.
*/
       else if ( winptr->typ == TYP_RWIN )
         {
         rwinpt = (WPRWIN *)winptr->ptr;
         if ( draw )
           {
           la_arr[0] = la;
           WPmodl_highlight(rwinpt,1,la_arr);
           WPsodl_all(rwinpt);
           }
         else
           {
           WPdodl_highlight(rwinpt);
           WPsodl_all(rwinpt);
           }
         }
       }
     }
/*
***The end.
*/
   return;
 }

/********************************************************/
/********************************************************/

        void WPerhg()

/*      Remove all highligt from all entities in all
 *      windows.
 *
 *      (C)2007-06-16 J. Kjellander
 *
 ******************************************************!*/
 {
   DBint   i,n;
   DFPOST *df;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;

/*
***Loop through all WPGWIN windows.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL )
       {
       if ( winptr->typ == TYP_GWIN ) 
         {
/*
***WPGWIN found, search through it's display file DF.
*/
         gwinpt = (WPGWIN *)winptr->ptr;

         n = 0;
         while ( n < gwinpt->df_ptr )
           {
/*
***If object is highlighted, turn highlight off.
*/
           df = gwinpt->df_adr + n;

          if ( (df+1)->la != DF_ERASED  &&  df->hdr.hili == TRUE )
             WPhgen(gwinpt->id.w_id,(df+1)->la,FALSE);
/*
***Next object in DF.
*/
          n += 3 + df->hdr.nvec;
           }
         }
       else if ( winptr->typ == TYP_RWIN ) 
         {
/*
***WPRWIN found. Delete all highlight lists for this window
***and reexecute main list.
*/
         rwinpt = (WPRWIN *)winptr->ptr;
         WPdodl_highlight(rwinpt);
         WPsodl_all(rwinpt);
         }
       }
     }
/*
***Also erase any pointermarks.
*/
   WPepmk(GWIN_ALL);
/*
***The end.
*/
    return;
 }

/********************************************************/
