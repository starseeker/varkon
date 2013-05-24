/**********************************************************************
*
*    gp14.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gpmeta();   Create plotfile
*    methdr();   Generate "GKS-Header item"
*    metwin();   Generate "Varkon window item"
*    metply();   Generate "GKS-Polyline item"
*    metfill();  Generate "GKS-Fill item"
*    metclr();   Generate "GKS-Clear workstation item"
*    metpen();   Generate "Varkon pen-number item"
*    metwdt();   Generate "Varkon linewidth item"
*    metend();   Generate "GKS-End item"
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
#include <time.h>

extern double  x[],y[];
extern char    a[];
extern int     ncrdxy;
extern bool    intrup,pltflg;
extern V3MDAT  sydata;
extern GMDATA  v3dbuf;

/*
***Med X11 har vi egen avbrottshantering.
*/
#ifdef V3_X11
extern short gptrty;
#endif

/*!******************************************************/

        short gpmeta(
        FILE   *filpek,
        VY     *plotvy,
        DBVector  *origo)

/*      Genererar plotfil på GKS metafil-format.
 *
 *      In:  filpek = Pekar på öppen plot-fil.
 *           plotvy = Plotområde.
 *           origo  = Plottens origo.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *          -1 => Avbrott från tangentbordet.
 *           GP0043 => Systemfel vid skapande av GKS metafil
 *
 *
 *      (C) Microform AB 4/8-85 Ulf Johansson
 *
 *      REVIDERAD:
 *
 *      24/10-85 Datum i header-item, Ulf Johansson
 *      2/12/85  Ändrat format i header-item, J. Kjellander
 *      12/12/85 Symbol, J. Kjellander
 *      28/9/86  IBM, J. Kjellander
 *      29/9/86  Ny nivåhant. R. Svedin
 *      19/10/86 Ny plottning, J. Kjellander
 *      22/10/86 v3dbuf, J. Kjellander
 *      30/10/86 Kurvnog., J. Kjellander
 *      4/11/86  Bug part, J. Kjellander
 *      7/5/87   Plotorigo, J. Kjellander
 *      28/8/87  Bplan, J. Kjellander
 *      17/2/88  Plotvy, J. Kjellander
 *      17/3/91  intrup, J. Kjellander
 *      1/12/92  Ytor, J. Kjellander
 *      9/6/93   Dynamiska segment, J. Kjellander
 *      1997-12-27 Linjebredd, J.Kjellander
 *      1999-12-14 Fill, J.Kjellander
 *      2004-07-19 Mesh, J.Kjellander, Örebro university
 *
 ******************************************************!*/
{
   DBptr   la;
   DBetype type;
   char    buf[MAXMETA];
   short   status,curpen;
   char    str[V3STRLEN+1];
   double  width,curwdt,tmpcn;
   DBId    dummy;
   METADEF md;
   GMUNON  gmpost;
   DBTmat  pmat;
   GMSEG  *sptarr[6],*segptr,arcseg[4];


/*
***Fördubbla kurvnoggrannheten och sätt plot-flaggan.
*/
    gpgtcn(&tmpcn);
    if ( 2*tmpcn < 100 ) gpstcn(2*tmpcn);
    else                 gpstcn(100.0);
    pltflg = TRUE;
/*
***Div. initiering.
*/
    curpen = -1;
    curwdt =  0;
    fseek(filpek,(long)0,0);
/*
***Generera "header item"
*/
    if ( (status=methdr(&md,filpek,buf)) < 0 ) return(status);
/*
***Generera "clear workstation item"
*/
    if ( (status=metclr(&md,filpek,buf)) < 0 ) return(status);
/*
***Generera "Varkon window item"
*/
    if ( (status=metwin(&md,filpek,buf,plotvy,origo)) < 0 )
      return(status);
/*
***Börja rita.
*/
    DBget_pointer('F',&dummy,&la,&type); /* Hämta LA och typ för huvud-parten. */
loop:
    while (DBget_pointer('N',&dummy,&la,&type) == 0)
      {
/*
***Interrupt från tangentbordet.
*/
      if ( intrup )
        { gpstcn(tmpcn); pltflg = FALSE;
        return(AVBRYT); }

#ifdef V3_X11
     if ( gptrty == X11  &&  wpintr() )
       {
       gpstcn(tmpcn);
       pltflg = FALSE;
       return(AVBRYT);
       }
#endif

      ncrdxy = 0;
      width  = 0;

      switch ( type )
        {
/*
***Punkt.
*/
        case POITYP:
        DBread_point(&gmpost.poi_un,la);
        gpdrpo(&gmpost.poi_un,la,CLIP);
        break;
/*
***Linje.
*/
        case LINTYP:
        DBread_line(&gmpost.lin_un,la);
        gpdrli(&gmpost.lin_un,la,CLIP);
        width = gmpost.lin_un.wdt_l;
        break;
/*
***Cirkelbåge.
*/
        case ARCTYP:
        DBread_arc(&gmpost.arc_un,arcseg,la);
        gpdrar(&gmpost.arc_un,arcseg,la,CLIP);
        width = gmpost.arc_un.wdt_a;
        break;
/*
***Kurva.
*/
        case CURTYP:
        DBread_curve(&gmpost.cur_un,&segptr,NULL,la);
        gpdrcu(&gmpost.cur_un,segptr,la,CLIP);
        width = gmpost.cur_un.wdt_cu;
        DBfree_segments(segptr);
        break;
/*
***Yta.
*/
        case SURTYP:
        DBread_surface(&gmpost.sur_un,la);
        DBread_srep_curves(&gmpost.sur_un,sptarr);
        gpdrsu(&gmpost.sur_un,sptarr,la,CLIP);
        DBfree_srep_curves(sptarr);
        break;
/*
***Koordinatsystem.
*/
        case CSYTYP:
        DBread_csys(&gmpost.csy_un,&pmat,la);
        gpdrcs(&gmpost.csy_un,&pmat,la,CLIP);
        break;
/*
***B_plan.
*/
        case BPLTYP:
        DBread_bplane(&gmpost.bpl_un,la);
        gpdrbp(&gmpost.bpl_un,la,CLIP);
        break;
/*
***Mesh.
*/
        case MSHTYP:
        DBread_mesh(&gmpost.msh_un,la,MESH_HEADER+MESH_VERTEX+MESH_HEDGE);
        gpdrms(&gmpost.msh_un,la,CLIP);
        break;
/*
***Text.
*/
        case TXTTYP:
        DBread_text(&gmpost.txt_un,str,la);
        gpdrtx(&gmpost.txt_un,str,la,CLIP);
        width = gmpost.txt_un.wdt_tx;
        break;
/*
***Längdmått.
*/
        case LDMTYP:
        DBread_ldim(&gmpost.ldm_un,la);
        gpdrld(&gmpost.ldm_un,la,CLIP);
        break;
/*
***Diametermått.
*/
        case CDMTYP:
        DBread_cdim(&gmpost.cdm_un,la);
        gpdrcd(&gmpost.cdm_un,la,CLIP);
        break;
/*
***Radiemått.
*/
        case RDMTYP:
        DBread_rdim(&gmpost.rdm_un,la);    
        gpdrrd(&gmpost.rdm_un,la,CLIP);
        break;
/*
***Vinkelmått.
*/
        case ADMTYP:
        DBread_adim(&gmpost.adm_un,la);
        gpdrad(&gmpost.adm_un,la,CLIP);
        break;
/*
***Snitt.
*/
        case XHTTYP:
        DBread_xhatch(&gmpost.xht_un,v3dbuf.crd,la);
        gpdrxh(&gmpost.xht_un,v3dbuf.crd,la,CLIP);
        break;
/*
***Part och grupp.
*/
        case PRTTYP:
        case GRPTYP:
        goto loop;
        break;
        }
/*
***Skriv till plotfil.
*/
      if ( ncrdxy > 0 )
        {
/*
***Ev. penna.
*/
        if (gmpost.hed_un.pen != curpen)
          {
          curpen = gmpost.hed_un.pen;
          if ( (status=metpen(&md,filpek,curpen,buf)) < 0 )
            return(status);
          }
/*
***Ev. linjebredd.
*/
        if (width != curwdt)
          {
          curwdt = width;
          if ( (status=metwdt(&md,filpek,curwdt,buf)) < 0 )
            return(status);
          }
/*
***Write polyline (GKS-11). Curvefont 3 = fill (GKS-14).
*/
        if ( type == CURTYP  &&  gmpost.cur_un.fnt_cu == 3 )
          {
          if ( (status=metfill(&md,filpek,ncrdxy-1,x,y,plotvy)) < 0 )
            return(status);
          }
        else
          {
          if ( (status=metply(&md,filpek,ncrdxy-1,x,y,a,plotvy,buf)) < 0 )
            return(status);
           }
        }
    }
/*
***Generate "end item"
*/
    if ( (status=metend(&md,filpek,buf)) < 0 ) return(status);
/*
***Reset curve accuracy.
*/
    gpstcn(tmpcn); pltflg = FALSE;
/*
***Slut.
*/
    return(0);
}

/********************************************************/
/*!******************************************************/

        short methdr(
        METADEF *mdp,
        FILE *filpek,
        char metarec[])

/*      Genererar "Header item"
 *
 *      In:  mdp     = Pekar på formatbeskrivning för metafil.
 *           filpek  = Pekar på öppen metafil.
 *           metarec = Plats för generering av item.
 *
 *      Ut: *mdp = Initierar md.
 *
 *      FV:  0 => Ok
 *           GP0043 => Systemfel vi skapande av GKS metafil
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

        short metwin(
        METADEF *mdp,
        FILE    *filpek,
        char     metarec[],
        VY      *plotvy,
        DBVector   *origo)

/*      Genererar "Window item"
 *
 *      In:
 *         mdp:      Pekar på formatbeskrivning för metafil.
 *         filpek:   Pekar på öppen metafil
 *         metarec:  Plats för generering av item
 *         plotvy:   Vy att plotta.
 *         origo:    Plotorigo.
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
    sprintf(&metarec[i],mdp->formtyp,VAR_WINDOW); i += mdp->typlen;
    sprintf(&metarec[i],mdp->formdat,4*mdp->fltlen); i += mdp->datlen;
    sprintf(&metarec[i],mdp->formflt,plotvy->vywin[ 0 ]);    /* x_min */
    i += mdp->fltlen;
    sprintf(&metarec[i],mdp->formflt,plotvy->vywin[ 2 ]);    /* x_max */
    i += mdp->fltlen;
    sprintf(&metarec[i],mdp->formflt,plotvy->vywin[ 1 ]);    /* y_min */
    i += mdp->fltlen;
    sprintf(&metarec[i],mdp->formflt,plotvy->vywin[ 3 ]);    /* y_max */
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

        short metply(
        METADEF *mdp,
        FILE  *filpek,
        int    k,
        double x[],
        double y[],
        char   a[],
        VY    *plotvy,
        char   metarec[])


/*      Genererar "GKS metafile polyline item" av en polyline
 *
 *      IN:
 *         mdp:      Pekar på formatbeskrivning för metafil.
 *         filpek:   Pekar på öppen metafil
 *         k:        Sist upptagna entry i polylinevektorerna
 *         x,y,a:    Polylinevektorerna (x-,y-koordinat och status)
 *         plotvy:   Plotområde.
 *         metarec:  Plats för generering av metafil
 *
 *      UT:
 *
 *      FV:  0 => Ok
 *           GP0043 => Systemfel vi skapande av GKS metafil
 *
 *      (c) Microform AB 4/8-85 Ulf Johansson
 *
 *      REVIDERAD:
 *
 *      16/10-85 Ulf Johansson
 *
 ******************************************************!*/
{
     register int i,j,l;
     char ch,*mp1,*mp2,*mp3;
     double normx,normy;
     double offsx,offsy;


     normx = plotvy->vywin[ 2 ] - plotvy->vywin[ 0 ];
     normy = plotvy->vywin[ 3 ] - plotvy->vywin[ 1 ];
     if (normx < normy)
          normx = normy;
     else
          normy = normx;
     offsx = plotvy->vywin[ 0 ];
     offsy = plotvy->vywin[ 1 ];




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

        short metfill(
        METADEF *mdp,
        FILE    *fp,
        int      k,
        double   x[],
        double   y[],
        VY      *plotvy)


/*      Generate "GKS metafile fill item".
 *
 *      In:
 *         mdp:      Format description
 *         fp:       Ptr to open plotfile
 *         k:        Offset to last point
 *         x,y:      Coordinates
 *         plotvy:   Plotarea
 *         metarec:  Buffer for output
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
   normx = plotvy->vywin[2] - plotvy->vywin[0];
   normy = plotvy->vywin[3] - plotvy->vywin[1];

   if ( normx < normy ) normx = normy;
   else                 normy = normx;

   offsx = plotvy->vywin[0];
   offsy = plotvy->vywin[1];
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

        short metclr(
        METADEF *mdp,
        FILE *filpek,
        char metarec[])

/*      Genererar "Clear workstation item"
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

        short metpen(
        METADEF *mdp,
        FILE *filpek,
        short pen,
        char metarec[])


/*      Genererar "Varkon pen-number item"
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

        short metwdt(
        METADEF *mdp,
        FILE *filpek,
        double wdt,
        char metarec[])


/*      Genererar "Varkon linewidth item"
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

        short metend(
        METADEF *mdp,
        FILE *filpek,
        char metarec[])

/*      Genererar "End item"
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
