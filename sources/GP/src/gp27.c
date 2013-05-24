/**********************************************************************
*
*    gp27.c
*    ======
*
*    This file is part of the VARKON Graphics Library.
*    URL: http://www.varkon.com
*
*    dxfout();    Create plotfile on DXF format
*    dxfopo();    Create point on DXF format
*    dxfoli();    Create line on DXF format
*    dxfoar();    Create arc on DXF format
*    dxfocu();    Create curve on DXF format
*    dxfocs();    Create coordinate system on DXF format
*    dxfopl();    Create polyline on DXF format
*    dxfobp();    Create B_plane on DXF format
*    dxfoms();    Create Mesh on DXF format
*    dxfotx();    Create text on DXF format
*    dxfold();    Create linear dimension on DXF format
*    dxfocd();    Create diameter dimension on DXF format
*    dxford();    Create radius dimension on DXF format
*    dxfoad();    Create angular dimension on DXF format
*    dxfoxh();    Create xhatch on DXF format
*    dxfniv();    Write level
*    dxfpen();    Write pen
*    dxfwdt();    Write width
*    dxffnt();    Write linetype
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
#include "../../GE/include/geotol.h"
#include "../include/GP.h"
#include <math.h>

extern double  x[],y[],z[];
extern char    a[];
extern int     ncrdxy;
extern bool    intrup,pltflg;
extern tbool   nivtb1[];
extern VY      actvy;
extern GMDATA  v3dbuf;
extern V3MDAT  sydata;

/*
***Med X11 har vi egen avbrottshantering.
*/
#ifdef V3_X11
extern short gptrty;
#endif

/*!******************************************************/

        short dxfout(
        FILE   *filpek,
        VY     *plotvy,
        DBVector  *origo)

/*      Genererar plotfil på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           plotvy = Plotområde.
 *           origo  = Pekar på nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *          -1 => Avbrott från tangentbordet.
 *           GP0043 => Systemfel.
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 *      1999-06-14 Comment+HEADER, J.Kjellander
 *      2004-07-18 Mesh, J.Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   DBptr   la;
   DBetype type;
   char    str[V3STRLEN+1];
   double  tmpcn;
   DBId    dummy;
   GMUNON  gmpost;
   DBTmat  pmat;
   GMSEG  *segptr,arcseg[4];

/*
***Fördubbla kurvnoggrannheten och sätt plot-flaggan.
*/
   gpgtcn(&tmpcn);
   if ( 2*tmpcn < 100.0 ) gpstcn(2*tmpcn);
   else                 gpstcn(100.0);
   pltflg = TRUE;   /* pltflg undertrycker text som linje */
/*
***Div. initiering.
*/
   fseek(filpek,(long)0,0);
/*
***Lets start with a comment.
*/
   fprintf(filpek,"999\nDXF-file created by Varkon %d.%d%c\n",
                  (int)sydata.vernr,(int)sydata.revnr,sydata.level);
/*
***Header section. AC1009 = R11.
*/
   fprintf(filpek,"  0\nSECTION\n  2\nHEADER\n");
   fprintf(filpek,"  9\n$ACADVER\n  1\nAC1009\n");
   fprintf(filpek,"  0\nENDSEC\n");
/*
***Entities section.
*/
   fprintf(filpek,"  0\nSECTION\n  2\nENTITIES\n");
/*
***Traversera GM.
*/
   DBget_pointer('F',&dummy,&la,&type);

   while ( DBget_pointer('N',&dummy,&la,&type) == 0 )
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
/*
***Processa.
*/
     switch(type)
       {
       case POITYP: DBread_point(&gmpost.poi_un,la);
                    dxfopo(filpek,&gmpost.poi_un,origo); break;

       case LINTYP: DBread_line(&gmpost.lin_un,la);
                    dxfoli(filpek,&gmpost.lin_un,origo); break;
 
       case ARCTYP: DBread_arc(&gmpost.arc_un,arcseg,la);
                    dxfoar(filpek,&gmpost.arc_un,arcseg,origo); break;

       case CURTYP: DBread_curve(&gmpost.cur_un,&segptr,NULL,la);
                    dxfocu(filpek,&gmpost.cur_un,segptr,origo);
                    DBfree_segments(segptr); break;

       case CSYTYP: DBread_csys(&gmpost.csy_un,&pmat,la);
                    dxfocs(filpek,&gmpost.csy_un,&pmat,origo); break;

       case BPLTYP: DBread_bplane(&gmpost.bpl_un,la);
                    dxfobp(filpek,&gmpost.bpl_un,origo); break;

       case MSHTYP: DBread_mesh(&gmpost.msh_un,la,MESH_HEADER+MESH_VERTEX+MESH_HEDGE);
                    dxfoms(filpek,&gmpost.msh_un,origo); break;

       case TXTTYP: DBread_text(&gmpost.txt_un,str,la);
                    dxfotx(filpek,&gmpost.txt_un,str,origo); break;

       case LDMTYP: DBread_ldim(&gmpost.ldm_un,la);
                    dxfold(filpek,&gmpost.ldm_un,origo); break;

       case CDMTYP: DBread_cdim(&gmpost.cdm_un,la);
                    dxfocd(filpek,&gmpost.cdm_un,origo); break;

       case RDMTYP: DBread_rdim(&gmpost.rdm_un,la);    
                    dxford(filpek,&gmpost.rdm_un,origo); break;

       case ADMTYP: DBread_adim(&gmpost.adm_un,la);
                    dxfoad(filpek,&gmpost.adm_un,origo); break;

       case XHTTYP: DBread_xhatch(&gmpost.xht_un,v3dbuf.crd,la);
                    dxfoxh(filpek,&gmpost.xht_un,v3dbuf.crd,origo); break;
       }
     }
/*
***End section.
*/
   fprintf(filpek,"  0\nENDSEC\n  0\nEOF\n");
/*
***Återställ kurvnoggrannhet mm.
*/
   gpstcn(tmpcn); pltflg = FALSE;
/*
***Slut.
*/
   return(0);
}

/********************************************************/
/*!******************************************************/

        short dxfopo(
        FILE   *filpek,
        GMPOI  *poipek,
        DBVector  *origo)

/*      Skriver ut punkt på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           poipek = Pekare till punktpost.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {

/*
***Synlig ?
*/
   if ( nivtb1[poipek->hed_p.level] ||
        poipek->hed_p.blank ) return(0);
/*
***Projicering.
*/
   gppltr(&(poipek->crd_p),x,y,z);
   ncrdxy = 1;
   if ( actvy.vydist != 0.0 ) gppstr(x,y,z);
/*
***Klippning.
*/
   if ( klpdot((short)-1,x,y) )
     {
/*
***Utskrift.
*/
     fprintf(filpek,"  0\nPOINT\n");
     dxfniv(filpek,poipek->hed_p.level);
     dxfpen(filpek,poipek->hed_p.pen);
     fprintf(filpek," 10\n%g\n",x[0]-origo->x_gm);
     fprintf(filpek," 20\n%g\n",y[0]-origo->y_gm);
     }

   return(0);
 }

/*******************************************************/
/*!******************************************************/

        short dxfoli(
        FILE   *filpek,
        GMLIN  *linpek,
        DBVector  *origo)

/*      Skriver ut linje på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           linpek = Pekare till linjepost.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {

/*
***Synlig ?
*/
   if ( nivtb1[linpek->hed_l.level] ||
        linpek->hed_l.blank ) return(0);
/*
***Projicering.
*/
   gpprli(linpek);
/*
***Klippning.
*/
   a[0] = 0; a[1] = (VISIBLE | ENDSIDE);
   if ( klplin((short)-1,x,y,a) )
     {
/*
***Utskrift.
*/
     fprintf(filpek,"  0\nLINE\n");
     dxffnt(filpek,linpek->fnt_l,linpek->lgt_l);
     dxfniv(filpek,linpek->hed_l.level);
     dxfpen(filpek,linpek->hed_l.pen);
     dxfwdt(filpek,linpek->wdt_l);
     fprintf(filpek," 10\n%g\n",x[0]-origo->x_gm);
     fprintf(filpek," 20\n%g\n",y[0]-origo->y_gm);
     fprintf(filpek," 11\n%g\n",x[1]-origo->x_gm);
     fprintf(filpek," 21\n%g\n",y[1]-origo->y_gm);
     }

   return(0);
 }

/*******************************************************/
/*!******************************************************/

        short dxfoar(
        FILE   *filpek,
        GMARC  *arcpek,
        GMSEG  *seg,
        DBVector  *origo)

/*      Skriver ut arc på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           arcpek = Pekare till arcpost.
 *           seg    = Pekare till segment.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (C) microform ab 27/3/91 J. Kjellander
 *
 *      1998-11-26 TOL7, J.Kjellander
 *      1999-02-11 Bug TOL/, J.Kjellander
 *
 ******************************************************!*/

 {
   int   k;
   short kstat;

/*
***Synlig ?
*/
   if ( nivtb1[arcpek->hed_a.level] ||
        arcpek->hed_a.blank ) return(0);
/*
***Generera vektorer.
*/
   k = -1;
   gpplar(arcpek,seg,&k,x,y,a);
/*
***Klipptest.
*/
   kstat = klpplq(-1,&k,x,y);
   if ( kstat > -1 )
     {
/*
***2D och helt synlig.
*/
     if ( arcpek->ns_a == 0  &&  kstat == 0 )
       {
       if ( ABS(arcpek->v2_a - arcpek->v1_a - 360.0) > TOL7 )
         fprintf(filpek,"  0\nARC\n");
       else
         fprintf(filpek,"  0\nCIRCLE\n");

       dxffnt(filpek,arcpek->fnt_a,arcpek->lgt_a);
       dxfniv(filpek,arcpek->hed_a.level);
       dxfpen(filpek,arcpek->hed_a.pen);
       dxfwdt(filpek,arcpek->wdt_a);

       fprintf(filpek," 10\n%g\n",arcpek->x_a - origo->x_gm);
       fprintf(filpek," 20\n%g\n",arcpek->y_a - origo->y_gm);
       fprintf(filpek," 40\n%g\n",arcpek->r_a);

       if ( ABS(arcpek->v2_a - arcpek->v1_a-360.0) > TOL7 )
         {
         fprintf(filpek," 50\n%g\n",arcpek->v1_a);
         fprintf(filpek," 51\n%g\n",arcpek->v2_a);
         }
       }
/*
***3D eller delvis synlig 2D.
*/
     else
       {
       if ( kstat == 1 ) klpply(-1,&k,x,y,a);
       dxfopl(filpek,&(arcpek->hed_a),k,x,y,a,origo);
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfocu(
        FILE   *filpek,
        GMCUR  *curpek,
        GMSEG  *seg,
        DBVector  *origo)

/*      Skriver ut kurva på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           curpek = Pekare till curpost.
 *           seg    = Pekare till segment.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 *      9/12/91 Anropet till gpplcu ändrat, J. Kjellander
 *
 ******************************************************!*/

 {
   int k;

/*
***Synlig ?
*/
   if ( nivtb1[curpek->hed_cu.level] ||
        curpek->hed_cu.blank ) return(0);
/*
***Generera vektorer.
*/
   k = -1;
   gpplcu(curpek,seg,&k,x,y,z,a);
/*
***Klipptest.
*/
   if ( klpply(-1,&k,x,y,a) )
     dxfopl(filpek,&(curpek->hed_cu),k,x,y,a,origo);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfocs(
        FILE   *filpek,
        GMCSY  *csypek,
        DBTmat *pmat,
        DBVector  *origo)

/*      Skriver ut koordinatsystem på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           csypek = Pekare till csypost.
 *           seg    = Pekare till segment.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {
   int k;

/*
***Synlig ?
*/
   if ( nivtb1[csypek->hed_pl.level] ||
        csypek->hed_pl.blank ) return(0);
/*
***Generera vektorer.
*/
   k = -1;
   gpplcs(csypek,pmat,&k,x,y,a);
/*
***Klipptest.
*/
   if ( klpply(-1,&k,x,y,a) )
     dxfopl(filpek,&(csypek->hed_pl),k,x,y,a,origo);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfobp(
        FILE   *filpek,
        GMBPL  *bplpek,
        DBVector  *origo)

/*      Skriver ut B-plane på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           bplpek = Pekare till bplpost.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {
   int k;

/*
***Synlig ?
*/
   if ( nivtb1[bplpek->hed_bp.level] ||
        bplpek->hed_bp.blank ) return(0);
/*
***Generera vektorer.
*/
   k = -1;
   gpplbp(bplpek,&k);
/*
***Klipptest.
*/
   if ( klpply(-1,&k,x,y,a) )
     dxfopl(filpek,&(bplpek->hed_bp),k,x,y,a,origo);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfoms(
        FILE     *filpek,
        DBMesh   *mshpek,
        DBVector *origo)

/*      Skriver ut Mesh på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           mshpek = Pekare till mshpost.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (C)18/7/2004 J. Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   int k;

/*
***Synlig ?
*/
   if ( nivtb1[mshpek->hed_m.level] ||
        mshpek->hed_m.blank ) return(0);
/*
***Generera vektorer.
*/
   k = -1;
   gpplms(mshpek,x,y,z,a,&k);
/*
***Klipptest.
*/
   if ( klpply(-1,&k,x,y,a) )
     dxfopl(filpek,&(mshpek->hed_m),k,x,y,a,origo);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfotx(
        FILE   *filpek,
        GMTXT  *txtpek,
        char    str[],
        DBVector  *origo)

/*      Skriver ut text på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           txtpek = Pekare till txtpost.
 *           str    = Pekare till sträng.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 *      1996-06-27 7-bitars till PC, J.Kjellander
 *      1998-10-01 3D-text, J.Kjellander
 *
 ******************************************************!*/

 {
   char  buf[V3STRLEN+1];
   int   k;
   short kstat;

/*
***Synlig ?
*/
   if ( nivtb1[txtpek->hed_tx.level] ||
        txtpek->hed_tx.blank ) return(0);
/*
***Generera vektorer.
*/
   k = -1;
   gppltx(txtpek,(unsigned char *)str,&k,x,y,z,a);
/*
***Klipptest.
*/
   kstat = klpplq(-1,&k,x,y);
   if ( kstat > -1 )
     {
/*
***Helt synlig.
*/
     if ( kstat == 0 )
       {
       fprintf(filpek,"  0\nTEXT\n");

       dxfniv(filpek,txtpek->hed_tx.level);
       dxfpen(filpek,txtpek->hed_tx.pen);

       fprintf(filpek," 10\n%g\n",txtpek->crd_tx.x_gm - origo->x_gm);
       fprintf(filpek," 20\n%g\n",txtpek->crd_tx.y_gm - origo->y_gm);
       fprintf(filpek," 40\n%g\n",txtpek->h_tx);
       fprintf(filpek," 41\n%g\n",txtpek->b_tx/57.0);
/*
***Mappa ev. gammal 7-bitars text till ISO. AutoCAD R13
***skapar ISO 8859 i sina DXF-filer.
*/
       strcpy(buf,str);

/* I en äkta 8-bitars värld behövs inte detta.

       i = 0;
       do
         {
         switch ( buf[i] )
           {
           case '}':  buf[i]=229; break;
           case '{':  buf[i]=228; break;
           case '|':  buf[i]=246; break;
           case ']':  buf[i]=197; break;
           case '[':  buf[i]=196; break;
           case '\\': buf[i]=214; break;
           }
         } while ( buf[i++] != '\0' );
*/

       fprintf(filpek,"  1\n%s\n",buf);
/*
***Vinkel och lutning.
*/
       fprintf(filpek," 50\n%g\n",txtpek->v_tx);
       fprintf(filpek," 51\n%g\n",txtpek->l_tx*0.45);
       }
/*
***Delvis synlig.
*/
     else
       {
       klpply(-1,&k,x,y,a);
       dxfopl(filpek,&(txtpek->hed_tx),k,x,y,a,origo);
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfold(
        FILE   *filpek,
        GMLDM  *ldmpek,
        DBVector  *origo)

/*      Skriver ut längdmått på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           ldmpek = Pekare till ldmpost.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {
   int k;

/*
***Synlig ?
*/
   if ( nivtb1[ldmpek->hed_ld.level] ||
        ldmpek->hed_ld.blank ) return(0);
/*
***Generera vektorer.
*/
   x[0] = ldmpek->p3_ld.x_gm;
   y[0] = ldmpek->p3_ld.y_gm;
   a[0] = 0;
   k = -1;
   gpplld(ldmpek,&k,x,y,a);
/*
***Klipptest.
*/
   if ( klpply(-1,&k,x,y,a) )
     dxfopl(filpek,&(ldmpek->hed_ld),k,x,y,a,origo);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfocd(
        FILE   *filpek,
        GMCDM  *cdmpek,
        DBVector  *origo)

/*      Skriver ut diametermått på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           cdmpek = Pekare till cdmpost.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {
   int k;

/*
***Synlig ?
*/
   if ( nivtb1[cdmpek->hed_cd.level] ||
        cdmpek->hed_cd.blank ) return(0);
/*
***Generera vektorer.
*/
   x[0] = cdmpek->p3_cd.x_gm;
   y[0] = cdmpek->p3_cd.y_gm;
   a[0] = 0;
   k = -1;
   gpplcd(cdmpek,&k,x,y,a);
/*
***Klipptest.
*/
   if ( klpply(-1,&k,x,y,a) )
     dxfopl(filpek,&(cdmpek->hed_cd),k,x,y,a,origo);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxford(
        FILE   *filpek,
        GMRDM  *rdmpek,
        DBVector  *origo)

/*      Skriver ut radiemått på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           rdmpek = Pekare till rdmpost.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {
   int   k;
   GMTXT txt;

/*
***Synlig ?
*/
   if ( nivtb1[rdmpek->hed_rd.level] ||
        rdmpek->hed_rd.blank ) return(0);
/*
***Generera vektorer.
*/
   k = -1;
   gpplrd(rdmpek,&txt,&k,x,y,a);
/*
***Klipptest.
*/
   if ( klpply(-1,&k,x,y,a) )
     dxfopl(filpek,&(rdmpek->hed_rd),k,x,y,a,origo);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfoad(
        FILE   *filpek,
        GMADM  *admpek,
        DBVector  *origo)

/*      Skriver ut längdmått på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           admpek = Pekare till admpost.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {
   int k;
   GMTXT txt;

/*
***Synlig ?
*/
   if ( nivtb1[admpek->hed_ad.level] ||
        admpek->hed_ad.blank ) return(0);
/*
***Generera vektorer.
*/
   k = -1;
   gpplad(admpek,&txt,&k,x,y,a);
/*
***Klipptest.
*/
   if ( klpply(-1,&k,x,y,a) )
     dxfopl(filpek,&(admpek->hed_ad),k,x,y,a,origo);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfoxh(
        FILE   *filpek,
        GMXHT  *xhtpek,
        gmflt   crd[],
        DBVector  *origo)

/*      Skriver ut snitt på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           xhtpek = Pekare till xhtpost.
 *           crd    = Pekare till koordinater.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {
   short i,n;

/*
***Synlig ?
*/
   if ( nivtb1[xhtpek->hed_xh.level] ||
        xhtpek->hed_xh.blank ) return(0);
/*
***Plocka snittlinjerna en och en, klipp dom och
***skriv ut som en linje.
*/
   i = 0;
   n = 4*xhtpek->nlin_xh;

   while (i < n)
     {
     x[0] = crd[i++];
     y[0] = crd[i++];
     a[0] = 0;
     x[1] = crd[i++];
     y[1] = crd[i++];
     a[1] = (VISIBLE | ENDSIDE);

     if ( klplin((short)-1,x,y,a) )
       {
       fprintf(filpek,"  0\nLINE\n");
       dxffnt(filpek,xhtpek->fnt_xh,xhtpek->lgt_xh);
       dxfniv(filpek,xhtpek->hed_xh.level);
       dxfpen(filpek,xhtpek->hed_xh.pen);

       fprintf(filpek," 10\n%g\n",x[0]-origo->x_gm);
       fprintf(filpek," 20\n%g\n",y[0]-origo->y_gm);
       fprintf(filpek," 11\n%g\n",x[1]-origo->x_gm);
       fprintf(filpek," 21\n%g\n",y[1]-origo->y_gm);
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfopl(
        FILE   *filpek,
        GMRECH *hedpek,
        int     n,
        double  x[],
        double  y[],
        char    a[],
        DBVector   *origo)

/*      Skriver ut polylinje på på DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           hedpek = Pekare till headerpost.
 *           n      = Antal vektorer.
 *           x,y    = Koordinater.
 *           a      = Vektorstatus.
 *           origo  = Nollpunkt.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 *      1996-06-25 Bug 66-grupp mm, J.Kjellander
 *
 ******************************************************!*/

 {
   int i=0;

/*
***POLYLINE.
*/
start:
   fprintf(filpek,"  0\nPOLYLINE\n");
   dxfniv(filpek,hedpek->level);
   dxfpen(filpek,hedpek->pen);
   fprintf(filpek," 66\n1\n");
/*
***VERTEX. Även varje VERTEX skall ha en layer-post.
*/
   do
     {
     fprintf(filpek,"  0\nVERTEX\n");
     dxfniv(filpek,hedpek->level);
     fprintf(filpek," 10\n%g\n",x[i] - origo->x_gm);
     fprintf(filpek," 20\n%g\n",y[i] - origo->y_gm);
     ++i;
     if (  i < n  &&  (a[i] & VISIBLE) != VISIBLE )
       {
       fprintf(filpek,"  0\nSEQEND\n");
       goto start;
       }
     } while ( i <= n );

   fprintf(filpek,"  0\nSEQEND\n");

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfniv(
        FILE   *filpek,
        short   niv)

/*      Skriver ut nivå.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           niv    = Nivå.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {
   fprintf(filpek,"  8\nLAY%d\n",niv);
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfpen(
        FILE   *filpek,
        short   pen)

/*      Skriver ut penna.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           pen    = penna.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {
   fprintf(filpek," 62\n%d\n",pen);
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxfwdt(
        FILE   *filpek,
        double  wdt)

/*      Skriver ut linjebredd.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           wdt    = Bredd 
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 1999-06-14 J. Kjellander
 *
 ******************************************************!*/

 {
   if ( wdt > 0.0 ) fprintf(filpek," 39\n%f\n",wdt);
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short dxffnt(
        FILE   *filpek,
        short   typ,
        gmflt   lgt)

/*      Skriver ut linjetyp.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           typ    = 0, 1 eller 2.
 *           lgt    = Ev. strecklängd.
 *
 *      Ut:  Inget.
 *
 *      FV:  0 => Ok
 *
 *      (C)microform ab 1996-07-04 J. Kjellander
 *
 ******************************************************!*/

 {
#define SNR_ROBUST 4141         /*  Robusts serienummer */

/*
***Vilken linjetyp är det ?
*/
   switch ( typ )
     {
/*
***Streckad. (Specialare för Robust i Nykroppa)
*/
     case 1:
     if ( sydata.sernr == SNR_ROBUST )
       {
       if      ( lgt < 5.0 )  fprintf(filpek,"  6\nBORDER\n");
       else if ( lgt <= 7.5 ) fprintf(filpek,"  6\nBORDER2\n");
       else                   fprintf(filpek,"  6\nBORDERX2\n");
       }
     else fprintf(filpek,"  6\nDSH%g\n",lgt);
     break;
/*
***Streck-prickad. (Specialare för Robust i Nykroppa)
*/
     case 2:
     if ( sydata.sernr == SNR_ROBUST )
       {
       if      ( lgt < 5.0 )  fprintf(filpek,"  6\nPHANTOM\n");
       else if ( lgt <= 7.5 ) fprintf(filpek,"  6\nPHANTOM2\n");
       else                   fprintf(filpek,"  6\nPHANTOMX2\n");
       }
     else fprintf(filpek,"  6\nCEN%g\n",lgt);
     break;

     default:
     break;
     }

   return(0);
 }

/********************************************************/
