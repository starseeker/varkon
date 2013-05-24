/**********************************************************************
*
*    wpplot.c
*    ========
*
*    This file is part of the VARKON WindowPac  Library.
*    URL: http://varkon.sourceforge.net
*
*    WPmkpf();             Make GKS meta plot file
*    WPgksm_header();      GKS Metafile output
*    WPgksm_window();             =""=
*    WPgksm_polyline();           =""=
*    WPgksm_fill():               =""=
*    WPgksm_clear();              =""=
*    WPgksm_pen();                =""=
*    WPgksm_width();              =""=
*    WPgksm_end();                =""=
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
#include "../../WP/include/WP.h"
#include <time.h>

extern V3MDAT  sydata;

/*!***************************************************************/

        short     WPmkpf(
        WPGWIN   *gwinpt,
        FILE     *filpek,
        VYWIN    *plotwin,
        DBVector *origo)

/*      Creates plotfile on GKS metafile format.
 *
 *      In:  gwinpt  = C ptr to plot window.
 *           filpek  = Ptr to open plot file.
 *           plotwin = Plot area.
 *           origo   = Plot origin.
 *
 *      Return:  0 => Ok
 *              -1 => Keyboard interrupt.
 *          GP0043 => System error.
 *
 *      (C)2006-12-25 Johan Kjellander
 *
 *      2007-01-09 pborder, piso,Sören L
 *      2007-09-13 WIDTH for dims, J.Kjellander
 *
 ******************************************************!*/
{
   double    x[PLYMXV],y[PLYMXV],z[PLYMXV],scale,size;
   char      a[PLYMXV];
   int       k;
   DBptr     la;
   DBetype   type;
   char      str[V3STRLEN+1],buf[MAXMETA];
   short     status,curpen;
   double    width,curwdt,tmpcn;
   DBId      dummy;
   DBAny     gmpost;
   DBTmat    pmat;
   DBSeg    *segptr,arcseg[4];
   DBSegarr *pborder,*piso;
   DBfloat   xhcrds[4*GMXMXL];
   DBCsys    csy;
   METADEF   md;

/*
***Increase curve accuracy 3 times during plot.
*/
    WPget_cacc(&tmpcn);
    if ( 3*tmpcn < 100 ) WPset_cacc(3*tmpcn);
    else                 WPset_cacc(100.0);
/*
***Current scale factor in this "window" can't be established
***in this situation since there is no "window" and we don't know
***what size the final output media will be. A good guess is that
***the "window" is a paper size A4 or A3, say 400 mm in X-dir.
***Point and Csys size is not scaled.
*/
    scale = (double)400.0/(plotwin->xmax - plotwin->xmin);
/*
***Other initializations.
*/
    curpen = -1;
    curwdt =  0;
    fseek(filpek,(long)0,0);
/*
***Make "header item"
*/
    if ( (status=WPgksm_header(&md,filpek,buf)) < 0 ) return(status);
/*
***Make "clear workstation item"
*/
    if ( (status=WPgksm_clear(&md,filpek,buf)) < 0 ) return(status);
/*
***Make "Varkon window item"
*/
    if ( (status=WPgksm_window(&md,filpek,buf,plotwin,origo)) < 0 )
      return(status);
/*
***Traverse DB.
*/
    DBget_pointer('F',&dummy,&la,&type);
loop:
    while (DBget_pointer('N',&dummy,&la,&type) == 0)
      {
      width  = 0;
      k = -1;
/*
***If blanked or on a blanked level, go on with the next entity.
*/
      DBread_header((DBHeader *)&gmpost,la);
      if ( !WPnivt(gwinpt->nivtab,gmpost.hed_un.level) || gmpost.hed_un.blank ) goto loop;

      switch ( type )
        {
/*
***Point, size = 1.0% of window size.
*/
        case POITYP:
        DBread_point(&gmpost.poi_un,la);
        size  = (double)0.01*(plotwin->xmax - plotwin->xmin);
        WPplpt(&gmpost.poi_un,size,&k,x,y,z,a);
        width = gmpost.poi_un.wdt_p;
        break;
/*
***Line.
*/
        case LINTYP:
        DBread_line(&gmpost.lin_un,la);
        WPplli(&gmpost.lin_un,&k,x,y,z,a);
        width = gmpost.lin_un.wdt_l;
        break;
/*
***Arc.
*/
        case ARCTYP:
        DBread_arc(&gmpost.arc_un,arcseg,la);
        WPplar(&gmpost.arc_un,arcseg,scale,&k,x,y,z,a);
        width = gmpost.arc_un.wdt_a;
        break;
/*
***Curve.
*/
        case CURTYP:
        DBread_curve(&gmpost.cur_un,&segptr,NULL,la);
        WPplcu(&gmpost.cur_un,segptr,scale,&k,x,y,z,a);
        DBfree_segments(segptr);
        width = gmpost.cur_un.wdt_cu;
        break;
/*
***Surface.
*/
        case SURTYP:
        DBread_surface(&gmpost.sur_un,la);
        DBread_sur_grwire(&gmpost.sur_un,&pborder,&piso);
        WPplsu(&gmpost.sur_un,pborder,piso,scale,&k,x,y,z,a);
        DBfree_sur_grwire(&gmpost.sur_un,pborder,piso);
        width = gmpost.sur_un.wdt_su;
        break;
/*
***Coordinate system. Size should be approx 15% of output media.
*/
        case CSYTYP:
        DBread_csys(&gmpost.csy_un,&pmat,la);
        size = (double)0.15*(plotwin->xmax - plotwin->xmin);
        WPplcs(&gmpost.csy_un,size,V3_CS_NORMAL,&k,x,y,z,a);
        break;
/*
***B_plane.
*/
        case BPLTYP:
        DBread_bplane(&gmpost.bpl_un,la);
        WPplbp(&gmpost.bpl_un,&k,x,y,z,a);
        width = gmpost.bpl_un.wdt_bp;
        break;
/*
***Mesh.
*/
        case MSHTYP:
        DBread_mesh(&gmpost.msh_un,la,MESH_HEADER+MESH_VERTEX+MESH_HEDGE);
        size  = (double)0.0075*(plotwin->xmax - plotwin->xmin);
        WPplms(&gmpost.msh_un,size,&k,x,y,z,a);
        width = gmpost.msh_un.wdt_m;
        break;
/*
***Text.
*/
        case TXTTYP:
        DBread_text(&gmpost.txt_un,str,la);
        WPpltx(&gmpost.txt_un,(unsigned char *)str,&k,x,y,z,a);
        width = gmpost.txt_un.wdt_tx;
        break;
/*
***Linear dimension.
*/
        case LDMTYP:
        DBread_ldim(&gmpost.ldm_un,&csy,la);
        WPplld(&gmpost.ldm_un,&csy,&k,x,y,z,a);
        width = gmpost.ldm_un.wdt_ld;
        break;
/*
***Diameter dimension.
*/
        case CDMTYP:
        DBread_cdim(&gmpost.cdm_un,&csy,la);
        WPplcd(&gmpost.cdm_un,&csy,&k,x,y,z,a);
        width = gmpost.cdm_un.wdt_cd;
        break;
/*
***Radius dimension.
*/
        case RDMTYP:
        DBread_rdim(&gmpost.rdm_un,&csy,la); 
        WPplrd(&gmpost.rdm_un,&csy,&k,x,y,z,a);
        width = gmpost.rdm_un.wdt_rd;
        break;
/*
***Angular dimension.
*/
        case ADMTYP:
        DBread_adim(&gmpost.adm_un,&csy,la);
        WPplad(&gmpost.adm_un,&csy,scale,&k,x,y,z,a);
        width = gmpost.adm_un.wdt_ad;
        break;
/*
***Hatch.
*/
        case XHTTYP:
        DBread_xhatch(&gmpost.xht_un,xhcrds,&csy,la);
        WPplxh(&gmpost.xht_un,xhcrds,&csy,&k,x,y,z,a);
        width = gmpost.xht_un.wdt_xh;
        break;
/*
***Part and group.
*/
        case PRTTYP:
        case GRPTYP:
        goto loop;
        break;
        }
/*
***Write to plot file.
*/
      if ( k > 0 )
        {
/*
***Project to window viewplane.
*/
        WPpply(gwinpt,k,x,y,z);
/*
***Clip to plotview borders and draw polyline if visible.
*/
        if ( WPcply(plotwin,(short)-1,&k,x,y,a) )
          {
/*
***Pen number.
*/
          if ( gmpost.hed_un.pen != curpen )
            {
            curpen = gmpost.hed_un.pen;
            if ( (status=WPgksm_pen(&md,filpek,curpen,buf) ) < 0 )
              return(status);
            }
/*
***Line width.
*/
          if ( width != curwdt )
            {
            curwdt = width;
            if ( (status=WPgksm_width(&md,filpek,curwdt,buf) ) < 0 )
              return(status);
            }
/*
***Write polyline (GKS-11). Curvefont 3 = fill (GKS-14).
*/
          if ( type == CURTYP  &&  gmpost.cur_un.fnt_cu == 3 )
            {
            if ( (status=WPgksm_fill(&md,filpek,k,x,y,plotwin)) < 0 )
              return(status);
            }
          else
            {
            if ( (status=WPgksm_polyline(&md,filpek,k,x,y,a,plotwin,buf)) < 0 )
              return(status);
            }
         }
       }
    }
/*
***Generate "end item"
*/
    if ( (status=WPgksm_end(&md,filpek,buf) ) < 0 ) return(status);
/*
**Reset curve accuracy.
*/
    WPset_cacc(tmpcn);
/*
***End.
*/
    return(0);
}

/********************************************************/
/*!******************************************************/

        short    WPgksm_header(
        METADEF *mdp,
        FILE    *filpek,
        char     metarec[])

/*      Write "Header item"
 *
 *      In:  mdp     = Pekar på formatbeskrivning för metafil.
 *           filpek  = Ptr to open file.
 *           metarec = Plats för generering av item.
 *
 *      Return:  0 => Ok
 *          GP0043 => System error.
 *
 *      (C)microform ab 27/2/89 J. Kjellander
 *
 ******************************************************!*/

{
   int    i,j;
   long   reltim;
   struct tm *timpek;

/*
***Metafil definitionspost.
*/
    mdp->typlen = TYPLEN;
    mdp->datlen = DATLEN;
    mdp->intlen = INTLEN;
    mdp->fltlen = FLTLEN; 
    sprintf(mdp->formtyp,"%%%d%c",mdp->typlen,'d');
    sprintf(mdp->formdat,"%%%d%c",mdp->datlen,'d');
    sprintf(mdp->formint,"%%%d%c",mdp->intlen,'d');
    sprintf(mdp->formflt,"%%%d%c%d%c",mdp->fltlen,'.',mdp->fltlen-2,'f');
/*
***Vad är klockan ?
*/
    reltim = time((long *)0);
    timpek = localtime(&reltim);
/*
***Generera "header item"
*/
    strcpy(metarec,"GKSM"); i = 4;
    sprintf(&metarec[i],"VARKON-3D/B V:%-5dR:%-5dS:%-5d 19%2d  ",
                sydata.vernr,sydata.revnr,sydata.sernr,timpek->tm_year);
    i += 40;
    sprintf(&metarec[i],"%2d%2d%2d%2d",timpek->tm_mon+1,
                timpek->tm_mday,timpek->tm_hour,timpek->tm_min);
    i += 8;

    sprintf(&metarec[i]," 1");                /* Version */
    i += 2;
    sprintf(&metarec[i]," 4");                /* Headerfältets längd */
    i += 2;
    sprintf(&metarec[i],"%2d",mdp->typlen);   /* Typfältets längd */
    i += 2;
    sprintf(&metarec[i],"%2d",mdp->datlen);   /* Datalängdfältets längd */
    i += 2;
    sprintf(&metarec[i],"%2d",mdp->intlen);   /* Ett heltals längd */
    i += 2;
    sprintf(&metarec[i],"%2d",mdp->fltlen);   /* Ett flyttals längd */
    i += 2;
    sprintf(&metarec[i]," 1");                /* ? 850212JK */
    i += 2;
    sprintf(&metarec[i]," 1");                /* ? 850212JK */
    i += 2;

    j = 0;
    while ( j < i )
      {
      if ( fwrite(&metarec[ j ],((i-j < 80) ? i-j : 80),1,filpek) < 0 )
        return(erpush("GP0043",""));
      if ( fwrite("\n",1,1,filpek) < 0 ) return(erpush("GP0043",""));
      j += 80;
      }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short     WPgksm_window(
        METADEF  *mdp,
        FILE     *filpek,
        char      metarec[],
        VYWIN    *plotwin,
        DBVector *origo)

/*      Write "Window item"
 *
 *      In:
 *         mdp:      Ptr to format descriptor.
 *         filpek:   Ptr. to open file.
 *         metarec:  Output buffer.
 *         plotwin:  Plot area.
 *         origo:    Plot origin.
 *
 *      Return:  0 => Ok
 *          GP0043 => System error.
 *
 *      (C)microform ab 27/2/89 J. Kjellander
 *
 ******************************************************!*/

{
   int i,j;
 
    strcpy(metarec,"GKSM"); i = 4;
    sprintf(&metarec[i],mdp->formtyp,VAR_WINDOW); i += mdp->typlen;
    sprintf(&metarec[i],mdp->formdat,4*mdp->fltlen); i += mdp->datlen;
    sprintf(&metarec[i],mdp->formflt,plotwin->xmin);    /* x_min */
    i += mdp->fltlen;
    sprintf(&metarec[i],mdp->formflt,plotwin->xmax);    /* x_max */
    i += mdp->fltlen;
    sprintf(&metarec[i],mdp->formflt,plotwin->ymin);    /* y_min */
    i += mdp->fltlen;
    sprintf(&metarec[i],mdp->formflt,plotwin->ymax);    /* y_max */
    i += mdp->fltlen;
    sprintf(&metarec[i],mdp->formflt,origo->x_gm);           /* origox */
    i += mdp->fltlen;
    sprintf(&metarec[i],mdp->formflt,origo->y_gm);           /* origoy */
    i += mdp->fltlen;
 
    j = 0;
    while ( j < i )
      {
      if ( fwrite(&metarec[ j ],((i-j < 80) ? i-j : 80),1,filpek) < 0 )
        return(erpush("GP0043",""));
      if ( fwrite("\n",1,1,filpek) < 0 ) return(erpush("GP0043",""));
      j += 80;
      }
 
    return(0);
}

/********************************************************/
/*!******************************************************/

        short    WPgksm_polyline(
        METADEF *mdp,
        FILE    *filpek,
        int      k,
        double   x[],
        double   y[],
        char     a[],
        VYWIN   *plotwin,
        char     metarec[])


/*      Write "GKS metafile polyline item".
 *
 *      IN:
 *         mdp:      Ptr to format descriptor.
 *         filpek:   Ptr to open file.
 *         k:        Sist upptagna entry i polylinevektorerna
 *         x,y,a:    Polylinevektorerna (x-,y-koordinat och status)
 *         plotwin:  Plot area.
 *         metarec:  Output buffer.
 *
 *      Return:  0 => Ok
 *          GP0043 => System error.
 *
 *      (c)2006-12-25 Johan Kjellander
 *
 ******************************************************!*/
{
     register int i,j,l;
     char ch,*mp1,*mp2,*mp3;
     double normx,normy;
     double offsx,offsy;


     normx = plotwin->xmax - plotwin->xmin;
     normy = plotwin->ymax - plotwin->ymin;

     if (normx < normy) normx = normy;
     else               normy = normx;

     offsx = plotwin->xmin;
     offsy = plotwin->ymin;

     j = 0;
     do {

                       /* Efterkommande koordinatpar måste vara synligt */
          while (j+1 <= k) {
               if ((a[ j+1 ] & VISIBLE) == VISIBLE)
                    break;
               j++;
          }

          if (j >= k)
               break;                    /* No more polylines */

          sprintf(metarec,"GKSM");                   /* Header */
          i = 4;
          sprintf(&metarec[ i ],mdp->formtyp,POLYLNITEM);  /* Polyline typ */
          i += mdp->typlen;
          mp1 = &metarec[ i ];             /* Markera position */
          i += mdp->datlen;                /* Reservera plats för itemlängd */
          mp2 = &metarec[ i ];             /* Markera position */
          i += mdp->intlen;                /* antal koordinatpar */
          mp3 = &metarec[ i ];             /* Markera position */



          l = 0;
          do {

               if ((i + mdp->fltlen + mdp->fltlen + 2) >= MAXMETA ) {
                    j--;               /* ta om */
                    break;             /* saknas plats i metarec */
               }

               l++;                    /* ett koordinatpar till */
               sprintf(&metarec[ i ],mdp->formflt,(x[ j ]-offsx)/normx);
               i += mdp->fltlen;
               sprintf(&metarec[ i ],mdp->formflt,(y[ j ]-offsy)/normy);
               i += mdp->fltlen;

               j++;
               if (j <= k)
                    if ((a[ j ] & VISIBLE) != VISIBLE )
                         break;             /* osynlig vektor */

          } while (j <= k);

          ch = *mp3;
          sprintf(mp2,mdp->formint,l); /* antal koordinater */
          *mp3 = ch;

          l = mdp->datlen + l*(mdp->fltlen + mdp->fltlen);
          ch = *mp2;
          sprintf(mp1,mdp->formdat,l);               /* itemlängd */
          *mp2 = ch;

          l = 0;
          while (l < i) {
               if (fwrite(&metarec[ l ],((i-l < 80) ? i-l : 80),1,filpek) < 0)
                    return(erpush("GP0043",""));
               if (fwrite("\n",1,1,filpek) < 0)               /* CR */
                    return(erpush("GP0043",""));
               l += 80;
          }


     } while (j <= k);

     return(0);
}
/********************************************************/
/*!******************************************************/

        short    WPgksm_fill(
        METADEF *mdp,
        FILE    *fp,
        int      k,
        double   x[],
        double   y[],
        VYWIN   *plotwin)

/*      Write "GKS metafile fill item".
 *
 *      In:
 *         mdp:      Format description
 *         fp:       Ptr to open plotfile
 *         k:        Offset to last point
 *         x,y:      Coordinates
 *         plotwin:  Plotarea
 *
 *      FV:  0      => Ok
 *           GP0043 => System error
 *
 *      (C) Microform AB 1999-12-14 J.Kjellander
 *
 ******************************************************!*/

 {
   int    pos,i;
   double normx,normy,offsx,offsy;

/*
***Normalise.
*/
   normx = plotwin->xmax - plotwin->xmin;
   normy = plotwin->ymax - plotwin->ymin;

   if ( normx < normy ) normx = normy;
   else                 normy = normx;

   offsx = plotwin->xmin;
   offsy = plotwin->ymin;
/*
***Header.
*/
   fprintf(fp,"%s","GKSM");
   fprintf(fp,mdp->formtyp,FILLITEM);
   fprintf(fp,mdp->formdat,(int)(6+2*(k+1)*10));
   fprintf(fp,mdp->formdat,(int)(k+1));

   pos = 4 + TYPLEN + DATLEN +DATLEN;
/*
***Polyline.
*/
   i = 0;

   while ( i<k+1 )
     {
     pos += FLTLEN;
     if ( pos > 80 )
       {
       fprintf(fp,"%c",'\n');
       pos = FLTLEN;
       }
     fprintf(fp,mdp->formflt,(x[i]-offsx)/normx);

     pos += FLTLEN;
     if ( pos > 80 )
       {
       fprintf(fp,"%c",'\n');
       pos = FLTLEN;
       }
     fprintf(fp,mdp->formflt,(y[i]-offsy)/normy);
   ++i;
     }
/*
***End.
*/
   fprintf(fp,"%c",'\n');

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short    WPgksm_clear(
        METADEF *mdp,
        FILE    *filpek,
        char     metarec[])

/*      Write "Clear workstation item"
 *
 *      In:
 *         mdp:      Pekar på formatbeskrivning för metafil.
 *         filpek:   Pekar på öppen metafil
 *         metarec:  Plats för generering av item
 *
 *      FV:  0 => Ok
 *           GP0043 => Systemfel vi skapande av GKS metafil
 *
 *      (C)microform ab 27/2/89 J. Kjellander
 *
 ******************************************************!*/

{
   int i,j;
 
    strcpy(metarec,"GKSM"); i = 4;
    sprintf(&metarec[i],mdp->formtyp,CLEARITEM); i += mdp->typlen;
    sprintf(&metarec[i],mdp->formdat,mdp->intlen); i += mdp->datlen;
    sprintf(&metarec[i],mdp->formint,0); i += mdp->intlen;

    j = 0;
    while ( j < i )
      {
      if ( fwrite(&metarec[j],((i-j < 80) ? i-j : 80),1,filpek) < 0 )
              return(erpush("GP0043",""));
      if ( fwrite("\n",1,1,filpek) < 0 ) return(erpush("GP0043",""));
      j += 80;
      }
 
    return(0);
}

/********************************************************/
/*!******************************************************/

        short    WPgksm_pen(
        METADEF *mdp,
        FILE    *filpek,
        short    pen,
        char     metarec[])

/*      Write "Varkon pen-number item"
 *
 *      IN:
 *         mdp:      Pekar på formatbeskrivning för metafil.
 *         filpek:   Pekar på öppen metafil
 *         pen:      Pen-nummer
 *         metarec:  Plats för generering av item
 *
 *      UT:
 *
 *      FV:  0 => Ok
 *           GP0043 => Systemfel vi skapande av GKS metafil
 *
 *      (c) Microform AB 4/8-85 Ulf Johansson
 *
 ******************************************************!*/

{
   int i,j;
 
    strcpy(metarec,"GKSM"); i = 4;
    sprintf(&metarec[i],mdp->formtyp,PENNRITEM); i += mdp->typlen;
    sprintf(&metarec[i],mdp->formdat,mdp->intlen); i += mdp->datlen;
    sprintf(&metarec[i],mdp->formint,pen); i += mdp->intlen;

    j = 0;
    while ( j < i )
      {
      if ( fwrite(&metarec[j],((i-j < 80) ? i-j : 80),1,filpek) < 0 )
              return(erpush("GP0043",""));
      if ( fwrite("\n",1,1,filpek) < 0 ) return(erpush("GP0043",""));
      j += 80;
      }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short    WPgksm_width(
        METADEF *mdp,
        FILE    *filpek,
        double   wdt,
        char     metarec[])

/*      Write "Varkon linewidth item"
 *
 *      IN:
 *         mdp:      Pekar på formatbeskrivning för metafil.
 *         filpek:   Pekar på öppen metafil
 *         wdt:      Linjebredd
 *         metarec:  Plats för generering av item
 *
 *      UT:
 *
 *      FV:  0 => Ok
 *           GP0043 => Systemfel vi skapande av GKS metafil
 *
 *      (c) Microform AB 1997-12-27 J.Kjellander
 *
 ******************************************************!*/

{
   int i,j;
 
    strcpy(metarec,"GKSM"); i = 4;
    sprintf(&metarec[i],mdp->formtyp,LINWDTITEM); i += mdp->typlen;
    sprintf(&metarec[i],mdp->formdat,mdp->fltlen); i += mdp->datlen;
    sprintf(&metarec[i],mdp->formflt,wdt); i += mdp->fltlen;

    j = 0;
    while ( j < i )
      {
      if ( fwrite(&metarec[j],((i-j < 80) ? i-j : 80),1,filpek) < 0 )
              return(erpush("GP0043",""));
      if ( fwrite("\n",1,1,filpek) < 0 ) return(erpush("GP0043",""));
      j += 80;
      }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short    WPgksm_end(
        METADEF *mdp,
        FILE    *filpek,
        char     metarec[])

/*      Write "End item"
 *
 *      In:
 *         mdp:      Pekar på formatbeskrivning för metafil.
 *         filpek:   Pekar på öppen metafil
 *         metarec:  Plats för generering av item
 *
 *      FV:  0 => Ok
 *           GP0043 => Systemfel vi skapande av GKS metafil
 *
 *      (C)microform ab 27/2/89 J. Kjellander
 *
 ******************************************************!*/

{
   int i,j;
 
    strcpy(metarec,"GKSM"); i = 4;
    sprintf(&metarec[i],mdp->formtyp,ENDITEM); i += mdp->typlen;
    sprintf(&metarec[i],mdp->formdat,0); i += mdp->datlen;

    j = 0;
    while ( j < i )
      {
      if ( fwrite(&metarec[j],((i-j < 80) ? i-j : 80),1,filpek) < 0 )
              return(erpush("GP0043",""));
      if ( fwrite("\n",1,1,filpek) < 0 ) return(erpush("GP0043",""));
      j += 80;
      }
 
    return(0);
}

/********************************************************/
