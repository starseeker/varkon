/*!******************************************************************/
/*  File: ig24.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  iggtmx();     Calculates window size to fit model               */
/*  igmsiz();     Calculates model size                             */
/*  igcdig();     Calibrates digitizer                              */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.varkon.com                                     */
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
/*  (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/IG.h"
#include "../../WP/include/WP.h"
#include "../../GP/include/GP.h"
#include <math.h>

extern double x[],y[];
extern double gpgszx,gpgszy;
extern short  gptrty;
extern int    ncrdxy;
extern bool   intrup;
extern GMDATA v3dbuf;
extern DIGDAT digdes;
extern DBTmat *lsyspk;

/*!******************************************************/

        short iggtmx(VY *pminvy)

/*      Beräknar "lagom" förnster som rymmer hela
 *      modellen.
 *
 *      In: Inget.
 *
 *      Ut: *pminvy = Vy som rymmer hela modellen.
 *
 *      FV:  0 = OK.
 *          -1 = Avbruten från tangentbordet.
 *
 *      (C)microform ab 28/9/86 J. Kjellander
 *
 *      27/8/87  B_PLANE, J. Kjellander
 *      19/3/91  <CTRL>c, J. Kjellander
 *      12/1/92  Streckade kurvor, J. Kjellander
 *      8/8/93   igmsiz(), J. Kjellander
 *
 ******************************************************!*/

 {
    short   status;
    double  sprop,dx,dy;

/*
***Beräkna modellens storlek.
*/
    if ( (status=igmsiz(pminvy)) < 0 ) return(status);
/*
***Kolla att fönstret inte blev orimligt litet. Kan tex.
***inträffa om modellen består av en enda punkt. Om så är
***fallet, returnera aktivt fönster.
*/
loop:
    dx = pminvy->vywin[2] - pminvy->vywin[0];
    dy = pminvy->vywin[3] - pminvy->vywin[1];

    if ( dx < 1e-10 && dy < 1e-10 )
      {
      gpgwin(pminvy);
      return(0);
      }

    if ( dx < 1e-10 )
      {
      pminvy->vywin[0] -= (0.05*dy);
      pminvy->vywin[2] += (0.05*dy);
      goto loop;
      }

    if ( dy < 1e-10 )
      {
      pminvy->vywin[1] -= (0.05*dx);
      pminvy->vywin[3] += (0.05*dx);
      goto loop;
      }
/*
***Korrigera fönstret map. skärmens proportioner.
*/
    sprop = gpgszy/gpgszx;

    if ( dy/dx > sprop )
      {
      pminvy->vywin[0] -= (dy/sprop-dx)/2.0;
      pminvy->vywin[2] += (dy/sprop-dx)/2.0;
      }
    else
      {
      pminvy->vywin[1] -= (sprop*dx-dy)/2.0;
      pminvy->vywin[3] += (sprop*dx-dy)/2.0;
      }
/*
***Gör fönstret 10% större så att allt syns ordentligt.
*/
    dx = pminvy->vywin[2] - pminvy->vywin[0];
    dy = pminvy->vywin[3] - pminvy->vywin[1];
    pminvy->vywin[0] -= (0.05*dx);
    pminvy->vywin[2] += (0.05*dx);
    pminvy->vywin[1] -= (0.05*dy);
    pminvy->vywin[3] += (0.05*dy);
/*
***Slut.
*/

    return(0);
 }

/********************************************************/
/*!******************************************************/

        short igmsiz(VY *pminvy)

/*      Beräknar modellens max och min-koordinater.
 *
 *      In: Inget.
 *
 *      Ut: *pminvy = Vy som rymmer hela modellen.
 *
 *      FV:  0 = OK.
 *          -1 = Avbruten från tangentbordet.
 *
 *      (C)microform ab 8/8/93 J. Kjellander
 *
 *      2004-07-10 Mesh, J.Kjellander, Örebro university
 *
 ******************************************************!*/

 {
    DBptr   la;
    DBetype typ;
    int     i;
    char    str[V3STRLEN+1];
    DBId    dummy;
    GMUNON  gmpost;
    DBTmat  pmat;
    GMSEG  *sptarr[6],*segptr,arcseg[4];

/*
***Sätt upp världens minsta fönster.
*/
    pminvy->vywin[0] = pminvy->vywin[1] =  1e20;
    pminvy->vywin[2] = pminvy->vywin[3] = -1e20;
/*
***Hämta LA och typ för huvud-parten.
*/
    DBget_pointer('F',&dummy,&la,&typ);
/*
***Loopa runt och hämta LA och typ för resten av GM.
*/
next:
    while ( DBget_pointer('N',&dummy,&la,&typ) == 0 )
      {
/*
***Interrupt från tangentbordet.
*/
      if ( intrup ) return(AVBRYT);

#ifdef V3_X11
      if ( gptrty == X11  &&  wpintr() ) return(AVBRYT);
#endif
/*
***För varje storhet, generera x,y på snabbaste
***sätt.
*/
      ncrdxy = 0;
      switch(typ)
        {
        case POITYP:
        DBread_point(&gmpost.poi_un,la);
        gpdrpo(&gmpost.poi_un,la,GEN);
        break;

        case LINTYP:
        DBread_line(&gmpost.lin_un,la);
        gmpost.lin_un.fnt_l = 0;
        gpdrli(&gmpost.lin_un,la,GEN);
        break;

        case ARCTYP:
        DBread_arc(&gmpost.arc_un,arcseg,la);
        gmpost.arc_un.fnt_a = 0;
        gpdrar(&gmpost.arc_un,arcseg,la,GEN);
        break;

        case CURTYP:
        DBread_curve(&gmpost.cur_un,&segptr,NULL,la);
        gmpost.cur_un.fnt_cu = 0;
        gpdrcu(&gmpost.cur_un,segptr,la,GEN);
        DBfree_segments(segptr);
        break;

        case SURTYP:
        DBread_surface(&gmpost.sur_un,la);
        DBread_srep_curves(&gmpost.sur_un,sptarr);
        gpdrsu(&gmpost.sur_un,sptarr,la,GEN);
        DBfree_srep_curves(sptarr);
        break;

        case CSYTYP:
        DBread_csys(&gmpost.csy_un,&pmat,la);
        gpdrcs(&gmpost.csy_un,&pmat,la,GEN);
        break;

        case BPLTYP:
        DBread_bplane(&gmpost.bpl_un,la);
        gpdrbp(&gmpost.bpl_un,la,GEN);
        break;

        case TXTTYP:
        DBread_text(&gmpost.txt_un,str,la);
        gpdrtx(&gmpost.txt_un,str,la,GEN);
        break;

        case LDMTYP:
        DBread_ldim(&gmpost.ldm_un,la);
        gmpost.ldm_un.auto_ld = FALSE;
        gpdrld(&gmpost.ldm_un,la,GEN);
        break;

        case CDMTYP:
        DBread_cdim(&gmpost.cdm_un,la);
        gmpost.cdm_un.auto_cd = FALSE;
        gpdrcd(&gmpost.cdm_un,la,GEN);
        break;

        case RDMTYP:
        DBread_rdim(&gmpost.rdm_un,la);
        gmpost.rdm_un.auto_rd = FALSE;
        gpdrrd(&gmpost.rdm_un,la,GEN);
        break;

        case ADMTYP:
        DBread_adim(&gmpost.adm_un,la);
        gmpost.adm_un.auto_ad = FALSE;
        gpdrad(&gmpost.adm_un,la,GEN);
        break;

        case XHTTYP:
        DBread_xhatch(&gmpost.xht_un,v3dbuf.crd,la);
        gmpost.xht_un.fnt_xh = 0;
        gpdrxh(&gmpost.xht_un,v3dbuf.crd,la,GEN);
        break;

        case MSHTYP:
        DBread_mesh(&gmpost.msh_un,la,MESH_HEADER);
        gmpost.msh_un.font_m = 1;
        gpdrms(&gmpost.msh_un,la,GEN);
        break;

        case PRTTYP:
        case GRPTYP:
        goto next;
        break;
        }
/*
***Sök igenom x och y efter max och min.
*/
    for ( i=0; i<ncrdxy; ++i )
      {
      if ( x[i] > pminvy->vywin[2] ) pminvy->vywin[2] = x[i];
      if ( x[i] < pminvy->vywin[0] ) pminvy->vywin[0] = x[i];

      if ( y[i] > pminvy->vywin[3] ) pminvy->vywin[3] = y[i];
      if ( y[i] < pminvy->vywin[1] ) pminvy->vywin[1] = y[i];
      }
/*
***Nästa storhet.
*/
    }

    return(0);
 }

/********************************************************/
/*!******************************************************/

        short igcdig()

/*      Varkon-funktion för att kalibrera digitizer.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0322 = Digitizer ej ansluten
 *
 *      (C)microform ab 28/3/88 J. Kjellander
 *
 ******************************************************!*/

 {
   short  status;
   gmflt  ddx,ddy,mdx,mdy,ddv,mdv,dv,rotddx;
   DBVector  d1,d2,m1,m2;
   DBTmat *tmplsy;

/*
***Är digitizer ansluten ?
*/
    if ( !digdes.def )
      {
      erpush("IG0322","");
      errmes();
      return(REJECT);
      }
/*
***Börja med att nollställa.
*/
   digdes.ldx = 0.0;
   digdes.ldy = 0.0;
   digdes.lmx = 0.0;
   digdes.lmy = 0.0;
   digdes.v = 0.0;
   digdes.s = 1.0;
/*
***Läs in kalibreringspunkter.
*/
   tmplsy = lsyspk;
   lsyspk = NULL;

   igptma(70,IG_MESS);
   gtpdig(&d1);
   igrsma();
   igptma(71,IG_MESS);
   gtpdig(&d2);
   igrsma();

   lsyspk = tmplsy;
/*
***Läs motsvarande XY-koordinater.
*/
   igptma(72,IG_MESS);
   status = genpov(&m1);
   igrsma();
   if ( status < 0 ) return(status);

   igptma(73,IG_MESS);
   status = genpov(&m2);
   igrsma();
   if ( status < 0 ) return(status);
/*
***Beräkna vinklar.
*/
   ddx = d2.x_gm - d1.x_gm;
   ddy = d2.y_gm - d1.y_gm;
   ddv = ATAN(ddy/ddx);

   mdx = m2.x_gm - m1.x_gm;
   mdy = m2.y_gm -m1.y_gm;
   mdv = ATAN(mdy/mdx);

   dv = mdv - ddv;
/*
***Rotera vinkeln dv.
*/
   rotddx = ddx*COS(dv) - ddy*SIN(dv);
/*
***Lagra värden i digdes.
*/
   digdes.ldx = d1.x_gm;
   digdes.ldy = d1.y_gm;
   digdes.lmx = m1.x_gm;
   digdes.lmy = m1.y_gm;
   digdes.v = dv;
   digdes.s = mdx/rotddx;

   return(0);
 }

/********************************************************/
