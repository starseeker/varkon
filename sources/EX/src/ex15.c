/*!*******************************************************
*
*    ex15.c
*    ======
*
*    This file includes:
*
*    EXmove();    Moves entities
*    EXmvla();    Moves one entity
*
*    EXcopy();    Copies entities
*    EXcpla();    Copies one entitiy
*
*    EXmirr();    Mirrors entities
*    EXmrla();    Mirrors one entitiy
*
*    EXrot();     Rotates entities
*    EXrola();    Rotates one entitiy
*
*    Note ! These functions are only used when the system
*           is in explicit mode.
*
*    This file is part of the VARKON Execute  Library.
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../include/EX.h"


extern GMDATA  v3dbuf;
extern V2NAPA  defnap;

/*!******************************************************/

        short EXmove(
        DBId      idmat[][MXINIV],
        DBshort   idant,
        DBVector *p1,
        DBVector *p2)

/*      Flyttar storheter.
 *
 *      In: idmat  = Identiteter.
 *          idant  = Antal storheter.
 *          p1,p2  = Bas- och deltaposition.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *
 *      Felkoder: EX1402 = Storheten finns ej.
 *
 *      (C)microform ab 16/3/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short    i;
    DBetype    typ;
    DBptr    la;
    DBTmat   tr;

/*
***Bilda transformationsmatris.
*/
    GEmktf_ts(p1,p2,1.0,1.0,1.0,&tr);
/*
***Loopa idant gånger.
*/
   for ( i=0; i<idant; ++i )
     {
     if ( DBget_pointer('I',&idmat[i][0],&la,&typ) < 0 )
       return(erpush("EX1402",""));
     EXmvla(la,typ,&tr);
     }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXmvla(
        DBptr   la,
        DBetype typ,
        DBTmat *ptr)

/*      Flyttar en storhet.
 *
 *      In: la    = Storhetens LA.
 *          typ   = Storhetens typ.
 *          ptr   = Pekare till translationsmatris.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 16/3/88 J. Kjellander
 *
 *      8/12/92 Nya transformationer, J. Kjellander
 *      8/6/93  Dynamiska kurvor, J. Kjellander
 *
 ******************************************************!*/

  {
    char     str[V3STRLEN+1];
    DBptr    la_tmp;
    short    i;
    GMRECH   hed;
    GMPRT    part;
    GMUNON   ogmpst;
    GMUNON   ngmpst;
    DBTmat   opmat;
    DBTmat   npmat;
    GMSEG   *osegpk,*nsegpk;
    GMSEG    aseg1[4],aseg2[4];

/*
***Sudda storheten ur GP.
*/
     EXdren(la,typ,FALSE,GWIN_ALL);
/*
***Gör förflyttning.
*/
    switch (typ)
      {
      case POITYP:
        DBread_point(&ogmpst.poi_un,la);
        DBread_point(&ngmpst.poi_un,la);
        GEtfPoint_to_local(&ogmpst.poi_un,ptr,&ngmpst.poi_un);
        DBupdate_point(&ngmpst.poi_un,la);
        break;

      case LINTYP:
        DBread_line(&ogmpst.lin_un,la);
        DBread_line(&ngmpst.lin_un,la);
        GEtfLine_to_local(&ogmpst.lin_un,ptr,&ngmpst.lin_un);
        DBupdate_line(&ngmpst.lin_un,la);
        break;

      case ARCTYP:
        DBread_arc(&ogmpst.arc_un,aseg1,la);
        DBread_arc(&ngmpst.arc_un,aseg2,la);
        GEtfArc_to_local(&ogmpst.arc_un,aseg1,ptr,&ngmpst.arc_un,aseg2);
        DBupdate_arc(&ngmpst.arc_un,aseg2,la);
        break;

      case CURTYP:
        DBread_curve(&ogmpst.cur_un,NULL,&osegpk,la);
        DBread_curve(&ngmpst.cur_un,NULL,&nsegpk,la);
        GEtfCurve_to_local(&ogmpst.cur_un,osegpk,NULL,ptr,
                           &ngmpst.cur_un,nsegpk,NULL);
        DBupdate_curve(&ngmpst.cur_un,nsegpk,la);
        DBfree_segments(osegpk);
        DBfree_segments(nsegpk);
        break;

      case CSYTYP:
        DBread_csys(&ogmpst.csy_un,&opmat,la);
        DBread_csys(&ngmpst.csy_un,&npmat,la);
        GEtfCsys_to_local(&ogmpst.csy_un,&opmat,ptr,&ngmpst.csy_un,&npmat);
        DBupdate_csys(&ngmpst.csy_un,&npmat,la);
        break;

      case TXTTYP:
        DBread_text(&ogmpst.txt_un,str,la);
        DBread_text(&ngmpst.txt_un,str,la);
        GEtfText_to_local(&ogmpst.txt_un,ptr,&ngmpst.txt_un);
        DBupdate_text(&ngmpst.txt_un,str,la);
        break;

      case LDMTYP:
        DBread_ldim(&ogmpst.ldm_un,la);
        DBread_ldim(&ngmpst.ldm_un,la);
        GEtfLdim_to_local(&ogmpst.ldm_un,ptr,&ngmpst.ldm_un);
        DBupdate_ldim(&ngmpst.ldm_un,la);
        break;

      case CDMTYP:
        DBread_cdim(&ogmpst.cdm_un,la);
        DBread_cdim(&ngmpst.cdm_un,la);
        GEtfCdim_to_local(&ogmpst.cdm_un,ptr,&ngmpst.cdm_un);
        DBupdate_cdim(&ngmpst.cdm_un,la);
        break;

      case RDMTYP:
        DBread_rdim(&ogmpst.rdm_un,la);
        DBread_rdim(&ngmpst.rdm_un,la);
        GEtfRdim_to_local(&ogmpst.rdm_un,ptr,&ngmpst.rdm_un);
        DBupdate_rdim(&ngmpst.rdm_un,la);
        break;

      case ADMTYP:
        DBread_adim(&ogmpst.adm_un,la);
        DBread_adim(&ngmpst.adm_un,la);
        GEtfAdim_to_local(&ogmpst.adm_un,ptr,&ngmpst.adm_un);
        DBupdate_adim(&ngmpst.adm_un,la);
        break;

      case XHTTYP:
        DBread_xhatch(&ngmpst.xht_un,v3dbuf.crd,la);
        GEtfHatch_to_local(&ngmpst.xht_un,v3dbuf.crd,ptr);
        DBupdate_xhatch(&ngmpst.xht_un,v3dbuf.crd,la);
        break;

      case PRTTYP:
        DBread_part(&part,la);

        for ( i=0; i<part.its_pt; ++i )
          {
          la_tmp = gmrdid(part.itp_pt,i);
          if ( la_tmp >= 0 )
            {
            do
              {
              DBread_header(&hed,la_tmp);
              EXmvla(la_tmp,hed.type,ptr);
              la_tmp = hed.n_ptr;
              }
            while ( la_tmp != DBNULL );
            }
          }
        break;
      }
/*
***Rita storheten igen.
*/
    EXdren(la,typ,TRUE,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXcopy(
        DBId      idmat[][MXINIV],
        DBshort   idant,
        DBVector *p1,
        DBVector *p2,
        DBshort   antal,
        bool      orgniv)

/*      Kopierar storheter.
 *
 *      In: idmat  = Identiteter.
 *          idant  = Antal storheter.
 *          p1,p2  = Bas- och deltaposition.
 *          antal  = Antal kopior.
 *          orgniv = TRUE=>Bibehåll ursprungliga nivåer.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *
 *      Felkoder: EX1402 = Storheten finns ej.
 *                EX1952 = Kan ej öppna part.
 *
 *      (C)microform ab 16/3/88 J. Kjellander
 *
 *      89/12/10 COPY-part, J. Kjellander
 *      8/12/92  Nya transformationer, J. Kjellander
 *
 ******************************************************!*/

  {
    short    i,j,status;
    DBetype    typ;
    DBptr    la;
    DBVector    tmp,delta;
    DBId     cpid,id;
    GMPRT    prt;
    GMPDAT   dat;
    DBTmat   tr;

/*
***Öppna en COPY-part.
*/
    prt.hed_pt.level = defnap.level;
    prt.hed_pt.pen = defnap.pen;
    prt.hed_pt.blank = defnap.blank;
    prt.hed_pt.hit = defnap.hit;
    strcpy(prt.name_pt,"COPY");
    prt.its_pt = idant+1;
    dat.mtyp_pd = 2;
    dat.matt_pd = BASIC;
    dat.npar_pd = 0;
    cpid.seq_val = iggnid();
    cpid.ord_val = 0;
    cpid.p_nextre = NULL;

    if ( EXoppt(&cpid,NULL,&prt,&dat,NULL,NULL) < 0 )
      return(erpush("EX1952",""));
/*
***Loopa idant gånger. För varje varv, bilda ny translations-
***matris.
*/
   for ( i=0; i<idant; ++i )
     {
     tmp.x_gm = p2->x_gm;
     tmp.y_gm = p2->y_gm;
     tmp.z_gm = p2->z_gm;
     delta.x_gm = p2->x_gm - p1->x_gm;
     delta.y_gm = p2->y_gm - p1->y_gm;
     delta.z_gm = p2->z_gm - p1->z_gm;
     GEmktf_ts(p1,&tmp,1.0,1.0,1.0,&tr);
/*
***COPY-parten kommer att bestå av idant storheter,
***var och en med lika många instanser som antal kopior.
*/
     if ( DBget_pointer('G',&idmat[i][0],&la,&typ) < 0 )
       {
       EXclpt();
       return(erpush("EX1402",""));
       }

     for ( j=0; j<antal; ++j )
       {
       id.seq_val = i+1;
       id.ord_val = 0;
       id.p_nextre = NULL;
       if ( (status=EXcpla(&id,la,typ,&tr,orgniv)) < 0 )
         {
         EXclpt();
         return(status);
         }
       tmp.x_gm += delta.x_gm;
       tmp.y_gm += delta.y_gm;
       tmp.z_gm += delta.z_gm;
       GEmktf_ts(p1,&tmp,1.0,1.0,1.0,&tr);
       }
     }
/*
***Stäng COPY-parten.
*/
    EXclpt();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXcpla(
        DBId    *id,
        DBptr    la,
        DBetype  typ,
        DBTmat  *ptr,
        bool     orgniv)

/*      Kopierar en storhet.
 *
 *      In: id     = Nya storhetens ID.
 *          la     = Storhetens LA.
 *          typ    = Storhetens typ.
 *          ptr    = Pekare till translationsmatris.
 *          orgniv = Ursprungliga nivåer ja/nej.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 16/3/88 J. Kjellander
 *
 *      19/3/89  Gamla postens attribut, J. Kjellander
 *      11/12/89 Orgniv, J. Kjellander
 *      8/12/92  Nya transformationer, J. Kjellander
 *
 ******************************************************!*/

  {
    char     str[V3STRLEN+1];
    DBptr    la_tmp;
    short    status,i;
    GMRECH   hed;
    GMPRT    part;
    GMPDAT   dat;
    GMUNON   ogmpst;
    GMUNON   ngmpst;
    DBTmat   opmat;
    DBTmat   npmat;
    V2NAPA   attr;
    DBId     cpid;
    GMSEG   *osegpk,*nsegpk;
    GMSEG    aseg1[4],aseg2[4];

/*
***Kopiera in aktiva attribut i "attr".
*/
   V3MOME(&defnap,&attr,sizeof(V2NAPA));
/*
***Läs storhetens header och ställ om header-attribut i
***"attr" därefter.
*/
    DBread_header(&hed,la);
    attr.pen = hed.pen;
    attr.hit = hed.hit;
    attr.blank = hed.blank;
    if ( orgniv ) attr.level = hed.level;
/*
***Kopiera.
*/
    switch (typ)
      {
      case POITYP:
        DBread_point(&ogmpst.poi_un,la);
        DBread_point(&ngmpst.poi_un,la);
        GEtfPoint_to_local(&ogmpst.poi_un,ptr,&ngmpst.poi_un);
        EXepoi(id,&ngmpst.poi_un,&attr);
        break;

      case LINTYP:
        DBread_line(&ogmpst.lin_un,la);
        DBread_line(&ngmpst.lin_un,la);
        GEtfLine_to_local(&ogmpst.lin_un,ptr,&ngmpst.lin_un);
        attr.lfont = ogmpst.lin_un.fnt_l;
        attr.ldashl = ogmpst.lin_un.lgt_l;
        EXelin(id,&ngmpst.lin_un,&attr);
        break;

      case ARCTYP:
        DBread_arc(&ogmpst.arc_un,aseg1,la);
        DBread_arc(&ngmpst.arc_un,aseg2,la);
        GEtfArc_to_local(&ogmpst.arc_un,aseg1,ptr,&ngmpst.arc_un,aseg2);
        attr.afont = ogmpst.arc_un.fnt_a;
        attr.adashl = ogmpst.arc_un.lgt_a;
        EXearc(id,&ngmpst.arc_un,aseg2,&attr);
        break;

      case CURTYP:
        DBread_curve(&ogmpst.cur_un,NULL,&osegpk,la);
        DBread_curve(&ngmpst.cur_un,NULL,&nsegpk,la);
        GEtfCurve_to_local(&ogmpst.cur_un,osegpk,NULL,ptr,
                           &ngmpst.cur_un,nsegpk,NULL);
        EXecur(id,&ngmpst.cur_un,nsegpk,nsegpk,&attr);
        DBfree_segments(osegpk);
        DBfree_segments(nsegpk);
        break;

      case CSYTYP:
        DBread_csys(&ogmpst.csy_un,&opmat,la);
        DBread_csys(&ngmpst.csy_un,&npmat,la);
        GEtfCsys_to_local(&ogmpst.csy_un,&opmat,ptr,&ngmpst.csy_un,&npmat);
        EXecsy(id,&ngmpst.csy_un,&npmat,&attr);
        break;

      case TXTTYP:
        DBread_text(&ogmpst.txt_un,str,la);
        DBread_text(&ngmpst.txt_un,str,la);
        GEtfText_to_local(&ogmpst.txt_un,ptr,&ngmpst.txt_un);
        attr.tsize = ogmpst.txt_un.h_tx;
        attr.twidth = ogmpst.txt_un.b_tx;
        attr.tslant = ogmpst.txt_un.l_tx;
        EXetxt(id,&ngmpst.txt_un,str,&attr);
        break;

      case LDMTYP:
        DBread_ldim(&ogmpst.ldm_un,la);
        DBread_ldim(&ngmpst.ldm_un,la);
        GEtfLdim_to_local(&ogmpst.ldm_un,ptr,&ngmpst.ldm_un);
        attr.dtsize = ogmpst.ldm_un.tsiz_ld;
        attr.dasize = ogmpst.ldm_un.asiz_ld;
        attr.dndig = ogmpst.ldm_un.ndig_ld;
        attr.dauto = ogmpst.ldm_un.auto_ld;
        EXeldm(id,&ngmpst.ldm_un,&attr);
        break;

      case CDMTYP:
        DBread_cdim(&ogmpst.cdm_un,la);
        DBread_cdim(&ngmpst.cdm_un,la);
        GEtfCdim_to_local(&ogmpst.cdm_un,ptr,&ngmpst.cdm_un);
        attr.dtsize = ogmpst.cdm_un.tsiz_cd;
        attr.dasize = ogmpst.cdm_un.asiz_cd;
        attr.dndig = ogmpst.cdm_un.ndig_cd;
        attr.dauto = ogmpst.cdm_un.auto_cd;
        EXecdm(id,&ngmpst.cdm_un,&attr);
        break;

      case RDMTYP:
        DBread_rdim(&ogmpst.rdm_un,la);
        DBread_rdim(&ngmpst.rdm_un,la);
        GEtfRdim_to_local(&ogmpst.rdm_un,ptr,&ngmpst.rdm_un);
        attr.dtsize = ogmpst.rdm_un.tsiz_rd;
        attr.dasize = ogmpst.rdm_un.asiz_rd;
        attr.dndig = ogmpst.rdm_un.ndig_rd;
        attr.dauto = ogmpst.rdm_un.auto_rd;
        EXerdm(id,&ngmpst.rdm_un,&attr);
        break;

      case ADMTYP:
        DBread_adim(&ogmpst.adm_un,la);
        DBread_adim(&ngmpst.adm_un,la);
        GEtfAdim_to_local(&ogmpst.adm_un,ptr,&ngmpst.adm_un);
        attr.dtsize = ogmpst.adm_un.tsiz_ad;
        attr.dasize = ogmpst.adm_un.asiz_ad;
        attr.dndig = ogmpst.adm_un.ndig_ad;
        attr.dauto = ogmpst.adm_un.auto_ad;
        EXeadm(id,&ngmpst.adm_un,&attr);
        break;

      case XHTTYP:
        DBread_xhatch(&ngmpst.xht_un,v3dbuf.crd,la);
        GEtfHatch_to_local(&ngmpst.xht_un,v3dbuf.crd,ptr);
        attr.xfont = ngmpst.xht_un.fnt_xh;
        attr.xdashl = ngmpst.xht_un.lgt_xh;
        EXexht(id,&ngmpst.xht_un,v3dbuf.crd,&attr);
        break;
/*
***Kopiera part.
*/
      case PRTTYP:
        DBread_part(&part,la);
        dat.mtyp_pd = 2;
        dat.matt_pd = BASIC;
        dat.npar_pd = 0;
        if ( EXoppt(id,NULL,&part,&dat,NULL,NULL) < 0 )
          return(erpush("EX1952",""));

        DBread_part(&part,la);
        for ( i=0; i<part.its_pt; ++i )
          {
          la_tmp = gmrdid(part.itp_pt,i);
          if ( la_tmp >= 0 )
            {
            do
              {
              DBread_header(&hed,la_tmp);
              cpid.seq_val = hed.seknr;
              cpid.ord_val = 0;
              cpid.p_nextre = NULL;
              if ( (status=EXcpla(&cpid,la_tmp,hed.type,ptr,orgniv)) < 0 )
                {
                EXclpt();
                return(status);
                }
              la_tmp = hed.n_ptr;
              }
            while ( la_tmp != DBNULL );
            }
          }
        EXclpt();
        break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXmirr(
        DBId      idmat[][MXINIV],
        DBshort   idant,
        DBVector *p1,
        DBVector *p2,
        bool      orgniv)

/*      Mirror entities.
 *
 *      In: idmat  = Identiteter.
 *          idant  = Antal storheter.
 *          p1,p2  = Speglingsaxel.
 *          orgniv = Urspr. nivåer j/n.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *
 *      Felkoder: EX1402 = Storheten finns ej.
 *
 *      (C)microform ab 16/11/88 J. Kjellander
 *
 *      89/12/10 MIRROR-part, J. Kjellander
 *
 ******************************************************!*/

  {
    short    i;
    DBetype  typ;
    DBptr    la;
    DBId     cpid,id;
    GMPRT    prt;
    GMPDAT   dat;

/*
***Öppna en MIRROR-part.
*/
    prt.hed_pt.level = defnap.level;
    prt.hed_pt.pen = defnap.pen;
    prt.hed_pt.blank = defnap.blank;
    prt.hed_pt.hit = defnap.hit;
    strcpy(prt.name_pt,"MIRROR");
    prt.its_pt = idant+1;
    dat.mtyp_pd = 2;
    dat.matt_pd = BASIC;
    dat.npar_pd = 0;
    cpid.seq_val = iggnid();
    cpid.ord_val = 0;
    cpid.p_nextre = NULL;

    if ( EXoppt(&cpid,NULL,&prt,&dat,NULL,NULL) < 0 )
      return(erpush("EX1952",""));
/*
***Loopa idant gånger.
*/
   for ( i=0; i<idant; ++i )
     {
     if ( DBget_pointer('G',&idmat[i][0],&la,&typ) < 0 )
       {
       EXclpt();
       return(erpush("EX1402",""));
       }

     id.seq_val = i+1;
     id.ord_val = 0;
     id.p_nextre = NULL;
     EXmrla(&id,la,typ,p1,p2,orgniv);
     }
/*
***Stäng MIRROR-parten.
*/
    EXclpt();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXmrla(
        DBId     *id,
        DBptr     la,
        DBetype   typ,
        DBVector *p1,
        DBVector *p2,
        bool      orgniv)

/*      Mirror single entity.
 *
 *      In: id     = Nya storhetens ID.
 *          la     = Storhetens LA.
 *          typ    = Storhetens typ.
 *          p1     = Punkt 1 på speglingsaxeln.
 *          p2     = Punkt 2 på speglingsaxeln.
 *          orgniv = Urspr. nivåer ja/nej.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 16/11/89 J. Kjellander
 *
 *      11/12/89 orgniv, J. Kjellander
 *      20/10/92 3D-cirklar, J. Kjellander
 *
 ******************************************************!*/

  {
    char     str[V3STRLEN+1];
    DBptr    la_tmp;
    short    status,i;
    GMRECH   hed;
    GMPRT    part;
    GMPDAT   dat;
    GMUNON   ogmpst;
    GMUNON   ngmpst;
    V2NAPA   attr;
    DBId     cpid;
    DBSeg    aseg1[4],aseg2[4];
    DBVector p3;
    DBTmat   t;

/*
***Kopiera in aktiva attribut i "attr".
*/
   V3MOME(&defnap,&attr,sizeof(V2NAPA));
/*
***Läs storhetens header och ställ om header-attribut i
***"attr" därefter.
*/
    DBread_header(&hed,la);
    attr.pen = hed.pen;
    attr.hit = hed.hit;
    attr.blank = hed.blank;
    if ( orgniv ) attr.level = hed.level;
/*
***Create mirror transformation.
*/
   p3.x_gm = p1->x_gm;
   p3.y_gm = p1->y_gm;
   p3.z_gm = p1->z_gm + 1.0;

   GEmktf_mirr(p1,p2,&p3,&t);
/*
***Spegla.
*/
    switch (typ)
      {
      case POITYP:
        DBread_point(&ogmpst.poi_un,la);
        DBread_point(&ngmpst.poi_un,la);
        GEtfPoint_to_local(&ogmpst.poi_un,&t,&ngmpst.poi_un);
        EXepoi(id,&ngmpst.poi_un,&attr);
        break;

      case LINTYP:
        DBread_line(&ogmpst.lin_un,la);
        DBread_line(&ngmpst.lin_un,la);
        GEtfLine_to_local(&ogmpst.lin_un,&t,&ngmpst.lin_un);
        attr.lfont = ogmpst.lin_un.fnt_l;
        attr.ldashl = ogmpst.lin_un.lgt_l;
        EXelin(id,&ngmpst.lin_un,&attr);
        break;
/*
***Är det en  3D-cirkel använder vi bara 2D-data.
***921020 JK.
*/
      case ARCTYP:
        DBread_arc(&ogmpst.arc_un,aseg1,la);
        DBread_arc(&ngmpst.arc_un,aseg2,la);
        ogmpst.arc_un.ns_a = ngmpst.arc_un.ns_a = 0;
        GEtfArc_to_local(&ogmpst.arc_un,aseg1,&t,&ngmpst.arc_un,aseg2);
        attr.afont = ogmpst.arc_un.fnt_a;
        attr.adashl = ogmpst.arc_un.lgt_a;
        EXearc(id,&ngmpst.arc_un,aseg2,&attr);
        break;

      case TXTTYP:
        DBread_text(&ogmpst.txt_un,str,la);
        DBread_text(&ngmpst.txt_un,str,la);
        GEtfText_to_local(&ogmpst.txt_un,&t,&ngmpst.txt_un);
        attr.tsize = ngmpst.txt_un.h_tx;
        attr.twidth = ngmpst.txt_un.b_tx;
        attr.tslant = ngmpst.txt_un.l_tx;
        EXetxt(id,&ngmpst.txt_un,str,&attr);
        break;

      case PRTTYP:
        DBread_part(&part,la);
        dat.mtyp_pd = 2;
        dat.matt_pd = BASIC;
        dat.npar_pd = 0;
        if ( EXoppt(id,NULL,&part,&dat,NULL,NULL) < 0 )
          return(erpush("EX1952",""));

        DBread_part(&part,la);
        for ( i=0; i<part.its_pt; ++i )
          {
          la_tmp = gmrdid(part.itp_pt,i);
          if ( la_tmp >= 0 )
            {
            do
              {
              DBread_header(&hed,la_tmp);
              cpid.seq_val = hed.seknr;
              cpid.ord_val = 0;
              cpid.p_nextre = NULL;
              if ( (status=EXmrla(&cpid,la_tmp,hed.type,p1,p2,orgniv)) < 0 )
                {
                EXclpt();
                return(status);
                }
              la_tmp = hed.n_ptr;
              }
            while ( la_tmp != DBNULL );
            }
          }
        EXclpt();
        break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXrot(
        DBId      idmat[][MXINIV],
        DBshort   idant,
        DBVector *p,
        gmflt     v)

/*      Roterar storheter.
 *
 *      In: idmat  = Identiteter.
 *          idant  = Antal storheter.
 *          p      = Basposition.
 *          v      = Vinkel.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *
 *      Felkoder: EX1402 = Storheten finns ej.
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    short    i;
    DBetype    typ;
    DBptr    la;
    DBVector    p2;
    DBTmat   tr; 

/*
***Bilda transformationsmatris för 2D-rotation i XY-planet.
*/
    p2.x_gm = p->x_gm;
    p2.y_gm = p->y_gm;
    p2.z_gm = p->z_gm + 1.0;
    GEmktf_rs(p,&p2,v,1.0,1.0,1.0,&tr);
/*
***Loopa idant gånger.
*/
   for ( i=0; i<idant; ++i )
     {
     if ( DBget_pointer('I',&idmat[i][0],&la,&typ) < 0 )
       return(erpush("EX1402",""));
     EXrola(la,typ,&tr);
     }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXrola(
        DBptr    la,
        DBetype  typ,
        DBTmat  *ptr)

/*      Roterar en storhet.
 *
 *      In: la    = Storhetens LA.
 *          typ   = Storhetens typ.
 *          ptr   = Pekare till transformationsmatris.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    char     str[V3STRLEN+1];
    DBptr    la_tmp;
    short    i;
    GMRECH   hed;
    GMPRT    part;
    GMUNON   ogmpst;
    GMUNON   ngmpst;
    DBTmat   opmat;
    DBTmat   npmat;
    GMSEG   *osegpk,*nsegpk;
    GMSEG    aseg1[4],aseg2[4];

/*
***Sudda storheten ur GP.
*/
     EXdren(la,typ,FALSE,GWIN_ALL);
/*
***Gör förflyttning.
*/
    switch (typ)
      {
      case POITYP:
        DBread_point(&ogmpst.poi_un,la);
        DBread_point(&ngmpst.poi_un,la);
        GEtfPoint_to_local(&ogmpst.poi_un,ptr,&ngmpst.poi_un);
        DBupdate_point(&ngmpst.poi_un,la);
        break;

      case LINTYP:
        DBread_line(&ogmpst.lin_un,la);
        DBread_line(&ngmpst.lin_un,la);
        GEtfLine_to_local(&ogmpst.lin_un,ptr,&ngmpst.lin_un);
        DBupdate_line(&ngmpst.lin_un,la);
        break;

      case ARCTYP:
        DBread_arc(&ogmpst.arc_un,aseg1,la);
        DBread_arc(&ngmpst.arc_un,aseg2,la);
        GEtfArc_to_local(&ogmpst.arc_un,aseg1,ptr,&ngmpst.arc_un,aseg2);
        DBupdate_arc(&ngmpst.arc_un,aseg2,la);
        break;

      case CURTYP:
        DBread_curve(&ogmpst.cur_un,NULL,&osegpk,la);
        DBread_curve(&ngmpst.cur_un,NULL,&nsegpk,la);
        GEtfCurve_to_local(&ogmpst.cur_un,osegpk,NULL,ptr,
                           &ngmpst.cur_un,nsegpk,NULL);
        DBupdate_curve(&ngmpst.cur_un,nsegpk,la);
        DBfree_segments(osegpk);
        DBfree_segments(nsegpk);
        break;

      case CSYTYP:
        DBread_csys(&ogmpst.csy_un,&opmat,la);
        DBread_csys(&ngmpst.csy_un,&npmat,la);
        GEtfCsys_to_local(&ogmpst.csy_un,&opmat,ptr,&ngmpst.csy_un,&npmat);
        DBupdate_csys(&ngmpst.csy_un,&npmat,la);
        break;

      case TXTTYP:
        DBread_text(&ogmpst.txt_un,str,la);
        DBread_text(&ngmpst.txt_un,str,la);
        GEtfText_to_local(&ogmpst.txt_un,ptr,&ngmpst.txt_un);
        DBupdate_text(&ngmpst.txt_un,str,la);
        break;

      case LDMTYP:
        DBread_ldim(&ogmpst.ldm_un,la);
        DBread_ldim(&ngmpst.ldm_un,la);
        GEtfLdim_to_local(&ogmpst.ldm_un,ptr,&ngmpst.ldm_un);
        DBupdate_ldim(&ngmpst.ldm_un,la);
        break;

      case CDMTYP:
        DBread_cdim(&ogmpst.cdm_un,la);
        DBread_cdim(&ngmpst.cdm_un,la);
        GEtfCdim_to_local(&ogmpst.cdm_un,ptr,&ngmpst.cdm_un);
        DBupdate_cdim(&ngmpst.cdm_un,la);
        break;

      case RDMTYP:
        DBread_rdim(&ogmpst.rdm_un,la);
        DBread_rdim(&ngmpst.rdm_un,la);
        GEtfRdim_to_local(&ogmpst.rdm_un,ptr,&ngmpst.rdm_un);
        DBupdate_rdim(&ngmpst.rdm_un,la);
        break;

      case ADMTYP:
        DBread_adim(&ogmpst.adm_un,la);
        DBread_adim(&ngmpst.adm_un,la);
        GEtfAdim_to_local(&ogmpst.adm_un,ptr,&ngmpst.adm_un);
        DBupdate_adim(&ngmpst.adm_un,la);
        break;

      case XHTTYP:
        DBread_xhatch(&ngmpst.xht_un,v3dbuf.crd,la);
        GEtfHatch_to_local(&ngmpst.xht_un,v3dbuf.crd,ptr);
        DBupdate_xhatch(&ngmpst.xht_un,v3dbuf.crd,la);
        break;

      case PRTTYP:
        DBread_part(&part,la);

        for ( i=0; i<part.its_pt; ++i )
          {
          la_tmp = gmrdid(part.itp_pt,i);
          if ( la_tmp >= 0 )
            {
            do
              {
              DBread_header(&hed,la_tmp);
              EXrola(la_tmp,hed.type,ptr);
              la_tmp = hed.n_ptr;
              }
            while ( la_tmp != DBNULL );
            }
          }
        break;
      }
/*
***Rita storheten igen.
*/
    EXdren(la,typ,TRUE,GWIN_ALL);

    return(0);
  }

/********************************************************/
