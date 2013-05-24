/*!******************************************************************/
/*  igattedit.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   igcpen();   Edit pen                                           */
/*   igcniv();   Edit level                                         */
/*   igcwdt();   Edit linewidth                                     */
/*   igcdal();   Edit dashlength                                    */
/*   igcfs();    Set font to solid                                  */
/*   igcfd();    Set font to dashed                                 */
/*   igcfc();    Set font to centreline                             */
/*   igcff();    Set font to filled                                 */
/*   igctsz();   Edit text size                                     */
/*   igctwd();   Edit text width                                    */
/*   igctsl();   Edit text slant                                    */
/*   igctfn();   Edit text font                                     */
/*   igctpm();   Edit text planemode                                */
/*   igcdts();   Edit dimension digitsize                           */
/*   igcdas();   Edit dimension arrowsize                           */
/*   igcdnd();   Edit dimension number of digits                    */
/*   igcda0();   Edit dimension auto = off                          */
/*   igcda1();   Edit dimension auto = on                           */
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
#include "../../GE/include/geotol.h"
#include "../include/IG.h"
#include "../../EX/include/EX.h"
#include "../../WP/include/WP.h"

extern short   v3mode;
extern DBptr   lsysla;

static short igcfnt(short font);
static short igcdau(short dauto);

/*!******************************************************/

        short igcpen()

/*      Varkonfunktion för att ändra en storhets penn-
 *      nummer.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3562 = Storheten ingår i en part
 *              IG3702 = Otillåtet pennummer
 *
 *      (C)microform ab 12/4/86 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *      11/11/86 Test om storhet ingår i part, J. Kjellander
 *      15/3/88  Ritpaketet, J. Kjellander
 *      18/11/88 getmid(), J. Kjellander
 *
 ******************************************************!*/

  {
    short   status,i,nid;
    DBetype   typ,typvek[IGMAXID];
    DBptr   la;
    pm_ptr  exnpt,retla,valtyp;
    char    istr[V3STRLEN+1];
    PMREFVA idmat[IGMAXID][MXINIV];
    DBHeader  hed;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på storheternas:s ID.
*/
loop:
    igptma(150,IG_MESS);
    typvek[0] = ALLTYP;
    nid = IGMAXID;
    if ( (status=getmid(idmat,typvek,&nid)) < 0 ) goto exit;
    igrsma();
/*
***Om basmodul, kolla att ingen av storheterna ingår i en part.
*/
    if ( v3mode & BAS_MOD )
      {
      for ( i=0; i<nid; ++i )
        if ( idmat[i][0].p_nextre != NULL )
          {
          erpush("IG3562","");
          errmes();
          WPerhg();
          goto loop;
         }
      }
/*
***Fråga efter nytt pennummer.
*/
    pmmark();
    if ( (status=genint(16,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Kolla att värdet är tillåtet.
*/
    if ( val.lit.int_va < 0 || val.lit.int_va > 32767 )
      {
      erpush("IG3702","");
      errmes();
      WPerhg();
      goto loop;
      }
/*
***Ändra alla storheterna.
*/
    for ( i=0; i<nid; ++i )
      {
/*
***Om basmodul, ändra pennummer i PM.
*/
      if ( v3mode & BAS_MOD ) pmchnp(idmat[i],PMPEN,exnpt,&retla);
/*
***Under alla omständigheter, ändra pennummer i GM och GP.
*/
      DBget_pointer('I',idmat[i],&la,&typ);
      DBread_header(&hed,la);
      if ( hed.pen != (short)val.lit.int_va )
        {
        EXdren(la,typ,FALSE,GWIN_ALL);
        hed.pen = (short)val.lit.int_va;
        DBupdate_header(&hed,la);
        EXdren(la,typ,TRUE,GWIN_ALL);
        }
      }
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcniv()

/*      Varkonfunktion för att ändra en storhets nivå.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3212 = Otillåtet nivånummer
 *              IG3562 = Någon av storh. ingår ipart
 *
 *      (C)microform ab 12/4/86 J. Kjellander
 *
 *      30/9/86  Ny nivåhantering, J. Kjellander
 *      6/10/86  GOMAIN, B. Doverud
 *      11/11/86 Test om storhet ingår i part, J. Kjellander
 *      15/3/88  Ritpaketet, J. Kjellander
 *      18/11/88 getmid(), J. Kjellander
 *      1998-01-12 Aktivt csys, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status,i,nid;
    DBetype   typ,typvek[IGMAXID];
    DBptr   la;
    char    istr[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idmat[IGMAXID][MXINIV];
    DBHeader  hed;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på storhetens:s ID.
*/
loop:
    igptma(151,IG_MESS);
    typvek[0] = ALLTYP;
    nid = IGMAXID;
    if ( (status=getmid(idmat,typvek,&nid)) < 0 ) goto exit;
    igrsma();
/*
***Om basmodul, kolla att ingen av storheterna ingår i en part.
*/
    if ( v3mode & BAS_MOD )
      {
      for ( i=0; i<nid; ++i )
        if ( idmat[i][0].p_nextre != NULL )
          {
          erpush("IG3562","");
          errmes();
          WPerhg();
          goto loop;
         }
      }
/*
***Fråga efter nytt nivånummer.
*/
    pmmark();
    if ( (status=genint(227,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Kolla att värdet är tillåtet.
*/
    if ( val.lit.int_va < 0 || val.lit.int_va > NT1SIZ-1 )
      {
      erpush("IG3212","");
      errmes();
      WPerhg();
      goto loop;
      }
/*
***Ändra alla storheterna.
*/
    for ( i=0; i<nid; ++i )
      {
/*
***Om basmodul, ändra nivånummer i PM.
*/
      if ( v3mode & BAS_MOD ) pmchnp(idmat[i],PMLEVEL,exnpt,&retla);
/*
***Ändra nivånummer i GM och GP.
*/
      DBget_pointer('I',idmat[i],&la,&typ);
      DBread_header(&hed,la);
      if ( hed.level != (short)val.lit.int_va )
        {
        EXdren(la,typ,FALSE,GWIN_ALL);
        hed.level = (short)val.lit.int_va;
        DBupdate_header(&hed,la);
        EXdren(la,typ,TRUE,GWIN_ALL);
        if ( la == lsysla ) igupcs(lsysla,V3_CS_ACTIVE);
        }
      }
/*
***Om igen.
*/
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcwdt()

/*      Varkonfunktion för att ändra en storhets 
 *      linjebredd.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3562 = Storheten ingår i en part
 *              IG3702 = Otillåtet pennummer
 *
 *      (C)microform ab 1998-11-25 J. Kjellander
 *
 *      1999-03-09, Bugfix pen, J.Kjellander
 *      2004-07-27 B_plane, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    short    status,i,nid;
    DBetype  typ,typvek[IGMAXID];
    DBptr    la;
    DBfloat    width;
    pm_ptr   exnpt,retla,valtyp;
    char     istr[V3STRLEN+1];
    DBPoint    poi;
    DBLine    lin;
    DBArc    arc;
    DBCurve    cur;
    DBText    txt;
    DBBplane bpl;
    PMREFVA  idmat[IGMAXID][MXINIV];
    DBHeader   hed;
    PMLITVA  val;

    static char dstr[V3STRLEN+1] = "0";

/*
***Ta reda på storheternas:s ID.
*/
loop:
    igptma(1633,IG_MESS);
    typvek[0] = POITYP+LINTYP+ARCTYP+CURTYP+TXTTYP+BPLTYP;
    nid = IGMAXID;
    if ( (status=getmid(idmat,typvek,&nid)) < 0 ) goto exit;
    igrsma();
/*
***Om basmodul, kolla att ingen av storheterna ingår i en part.
*/
    if ( v3mode & BAS_MOD )
      {
      for ( i=0; i<nid; ++i )
        if ( idmat[i][0].p_nextre != NULL )
          {
          erpush("IG3562","");
          errmes();
          WPerhg();
          goto loop;
         }
      }
/*
***Fråga efter ny linjebredd.
*/
askwdt:
    pmmark();
    if ( (status=genflt(1632,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket.
*/
    inevev(exnpt,&val,&valtyp);

    if ( val.lit_type == C_INT_VA ) width = (DBfloat)val.lit.int_va;
    else width = val.lit.float_va;
/*
***Kolla att värdet är tillåtet.
*/
    if ( width < 0.0 )
      {
      pmrele();
      erpush("IG3872","");
      errmes();
      goto askwdt;
      }
/*
***Ändra alla storheterna.
*/
    for ( i=0; i<nid; ++i )
      {
/*
***Om basmodul, ändra pennummer i PM.
*/
      if ( v3mode & BAS_MOD ) pmchnp(idmat[i],PMWIDTH,exnpt,&retla);
/*
***Under alla omständigheter, ändra pennummer i GM och GP.
*/
      DBget_pointer('I',idmat[i],&la,&typ);
      DBread_header(&hed,la);
      EXdren(la,typ,FALSE,GWIN_ALL);

      if ( typ == POITYP )
        {
        DBread_point(&poi,la);
        poi.wdt_p = width;
        DBupdate_point(&poi,la);
        }
      else if ( typ == LINTYP )
        {
        DBread_line(&lin,la);
        lin.wdt_l = width;
        DBupdate_line(&lin,la);
        }
      else if ( typ == ARCTYP )
        {
        DBread_arc(&arc,NULL,la);
        arc.wdt_a = width;
        DBupdate_arc(&arc,NULL,la);
        }
      else if ( typ == CURTYP )
        {
        DBread_curve(&cur,NULL,NULL,la);
        cur.wdt_cu = width;
        DBupdate_curve(&cur,NULL,la);
        }
      else if ( typ == TXTTYP )
        {
        DBread_text(&txt,NULL,la);
        txt.wdt_tx = width;
        DBupdate_text(&txt,NULL,la);
        }
      else if ( typ == BPLTYP )
        {
        DBread_bplane(&bpl,la);
        bpl.wdt_bp = width;
        DBupdate_bplane(&bpl,la);
        }


      EXdren(la,typ,TRUE,GWIN_ALL);
      }
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcdal()

/*      Varkonfunktion för att ändra en linjes,
 *      cirkels, kurvas eller snitts strecklängd.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Storheten ingår i en part.
 *              IG3802 = Storheten är heldragen.
 *              IG3862 = Otillåten strecklängd.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      17/3/88  Snitt, J. Kjellander
 *      12/1/92  Kurvor, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    short   font;
    DBetype   typ;
    DBptr   la;
    DBfloat   dashl;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBLine   lin;
    DBArc   arc;
    DBCurve   cur;
    DBHatch   xht;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på storhetens:s ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = LINTYP+ARCTYP+CURTYP+XHTTYP;
    if ( (status=getidt(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();
/*
***Kolla att storheten inte ingår i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Kolla att storheten inte är heldragen.
*/
    DBget_pointer('I',idvek,&la,&typ);

    if ( typ == LINTYP )
      {
      DBread_line(&lin,la);
      font = lin.fnt_l;
      dashl = lin.lgt_l;
      }
    else if ( typ == ARCTYP )
      {
      DBread_arc(&arc,NULL,la);
      font = arc.fnt_a;
      dashl = arc.lgt_a;
      }
    else if ( typ == CURTYP )
      {
      DBread_curve(&cur,NULL,NULL,la);
      font = cur.fnt_cu;
      dashl = cur.lgt_cu;
      }
    else
      {
      DBread_xhatch(&xht,NULL,la);
      font = xht.fnt_xh;
      dashl = xht.lgt_xh;
      }

    if ( font == 0 )
      {
      erpush("IG3802","");
      errmes();
      goto loop;
      }
/*
***Fråga efter ny strecklängd.
*/
askdsh:
    sprintf(strbuf,"%s%g  %s",iggtts(12),dashl,iggtts(248));
    igplma(strbuf,IG_INP);

    pmmark();
    if ( (status=genflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket och beräkna ny strecklängd.
*/
    inevev(exnpt,&val,&valtyp);

    if ( val.lit_type == C_INT_VA ) dashl = (DBfloat)val.lit.int_va;
    else dashl = val.lit.float_va;
/*
***Kolla att det är ett rimligt värde.
*/
    if ( dashl < 2*TOL2 )
      {
      pmrele();
      erpush("IG3862","");
      errmes();
      goto askdsh;
      }
/*
***Ändra strecklängd i PM, GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( typ == LINTYP )
      {
      if ( v3mode & BAS_MOD ) pmchnp(idvek,PMLDASHL,exnpt,&retla);
      lin.lgt_l = dashl;
      DBupdate_line(&lin,la);
      }
    else if ( typ == ARCTYP )
      {
      if ( v3mode & BAS_MOD ) pmchnp(idvek,PMADASHL,exnpt,&retla);
      arc.lgt_a = dashl;
      DBupdate_arc(&arc,NULL,la);
      }
    else if ( typ == CURTYP )
      {
      if ( v3mode & BAS_MOD ) pmchnp(idvek,PMCDASHL,exnpt,&retla);
      cur.lgt_cu = dashl;
      DBupdate_curve(&cur,NULL,la);
      }
    else
      {
      if ( v3mode & BAS_MOD ) pmchnp(idvek,PMXDASHL,exnpt,&retla);
      xht.lgt_xh = dashl;
      DBupdate_xhatch(&xht,NULL,la);
      }

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcfs()

/*      Varkonfunktion för att ändra en storhets
 *      font till heldraget.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      17/3/88  Snitt, J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcfnt((short)0));
   }

/********************************************************/
/*!******************************************************/

        short igcfd()

/*      Varkonfunktion för att ändra en storhets
 *      font till streckat.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      17/3/88  Snitt, J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcfnt((short)1));
   }

/********************************************************/
/*!******************************************************/

        short igcfc()

/*      Varkonfunktion för att ändra en storhets
 *      font till streckprickat.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      17/3/88  Snitt, J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcfnt((short)2));
   }

/********************************************************/
/*!******************************************************/

        short igcff()

/*      Interactive edit entity font to 3 = filled.
 *
 *      (C)microform ab 1999-12-15 J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcfnt((short)3));
   }

/********************************************************/
/*!******************************************************/

static short igcfnt(short font)

/*      Ändrar en linjes, cirkels, kurvas eller snitts font.
 *
 *      In: font = 0,1 eller 2.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Storheten ingår i en part.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      17/3/88  Snitt, J. Kjellander
 *      12/1/92  Kurvor, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    bool    end,right;
    pm_ptr  exnpt,retla;
    PMREFVA idvek[MXINIV];
    DBLine   lin;
    DBArc   arc;
    DBCurve   cur;
    DBHatch   xht;
    PMLITVA val;

/*
***Ta reda på storhetens:s ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = LINTYP+ARCTYP+CURTYP+XHTTYP;
    if ( (status=getidt(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();
/*
***Kolla att storheten inte ingår i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Ändra font i PM, GM och GP.
*/
    DBget_pointer('I',idvek,&la,&typ);

    EXdren(la,typ,FALSE,GWIN_ALL);

    val.lit_type = C_INT_VA;
    val.lit.int_va = font;
    pmclie(&val,&exnpt);

    if ( typ == LINTYP )
      {
      if ( v3mode & BAS_MOD ) pmchnp(idvek,PMLFONT,exnpt,&retla);
      DBread_line(&lin,la);
      lin.fnt_l = font;
      DBupdate_line(&lin,la);
      }
    else if ( typ == ARCTYP )
      {
      if ( v3mode & BAS_MOD ) pmchnp(idvek,PMAFONT,exnpt,&retla);
      DBread_arc(&arc,NULL,la);
      arc.fnt_a = font;
      DBupdate_arc(&arc,NULL,la);
      }
    else if ( typ == CURTYP )
      {
      if ( v3mode & BAS_MOD ) pmchnp(idvek,PMCFONT,exnpt,&retla);
      DBread_curve(&cur,NULL,NULL,la);
      cur.fnt_cu = font;
      DBupdate_curve(&cur,NULL,la);
      }
    else
      {
      if ( v3mode & BAS_MOD ) pmchnp(idvek,PMXFONT,exnpt,&retla);
      DBread_xhatch(&xht,NULL,la);
      xht.fnt_xh = font;
      DBupdate_xhatch(&xht,NULL,la);
      }

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igctsz()

/*      Varkonfunktion för att ändra en texthöjd.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Texten ingår i en part.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBText   txt;
    char    str[V3STRLEN+1];
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på textens:s ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = TXTTYP;
    if ( (status=getidt(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();
/*
***Kolla att texten inte ingår i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Läs GM-data.
*/
    DBget_pointer('I',idvek,&la,&typ);

    DBread_text(&txt,str,la);
/*
***Fråga efter ny texthöjd.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(11),txt.h_tx,iggtts(248));
    igplma(strbuf,IG_INP);

    pmmark();
    if ( (status=genflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }

    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket och beräkna ny texthöjd.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Ändra texthöjd i PM, GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( v3mode & BAS_MOD ) pmchnp(idvek,PMTSIZE,exnpt,&retla);
    if ( val.lit_type == C_INT_VA ) txt.h_tx = val.lit.int_va;
    else txt.h_tx = val.lit.float_va;
    DBupdate_text(&txt,str,la);

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igctwd()

/*      Varkonfunktion för att ändra en textbredd.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Texten ingår i en part.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBText   txt;
    char    str[V3STRLEN+1];
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på textens:s ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = TXTTYP;
    if ( (status=getidt(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();
/*
***Kolla att texten inte ingår i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Läs GM-data.
*/
    DBget_pointer('I',idvek,&la,&typ);

    DBread_text(&txt,str,la);
/*
***Fråga efter ny textbredd.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(13),txt.b_tx,iggtts(248));
    igplma(strbuf,IG_INP);

    pmmark();
    if ( (status=genflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket och beräkna ny textbredd.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Ändra textbredd i PM, GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( v3mode & BAS_MOD ) pmchnp(idvek,PMTWIDTH,exnpt,&retla);
    if ( val.lit_type == C_INT_VA ) txt.b_tx = val.lit.int_va;
    else txt.b_tx = val.lit.float_va;
    DBupdate_text(&txt,str,la);

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igctsl()

/*      Varkonfunktion för att ändra en text-slant.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Texten ingår i en part.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBText   txt;
    char    str[V3STRLEN+1];
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på textens:s ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = TXTTYP;
    if ( (status=getidt(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();
/*
***Kolla att texten inte ingår i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Läs GM-data.
*/
    DBget_pointer('I',idvek,&la,&typ);

    DBread_text(&txt,str,la);
/*
***Fråga efter ny textslant.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(15),txt.l_tx,iggtts(248));
    igplma(strbuf,IG_INP);

    pmmark();
    if ( (status=genflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket och beräkna ny textslant.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Ändra textslant i PM, GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( v3mode & BAS_MOD ) pmchnp(idvek,PMTSLANT,exnpt,&retla);
    if ( val.lit_type == C_INT_VA ) txt.l_tx = val.lit.int_va;
    else txt.l_tx = val.lit.float_va;
    DBupdate_text(&txt,str,la);

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igctfn()

/*      Varkon-funktion för att ändra en texts font.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Storheten ingår i en part.
 *
 *      (C)microform ab 28/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    bool    end,right;
    pm_ptr  exnpt,retla,valtyp;
    int     font;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    PMREFVA idvek[MXINIV];
    DBText   txt;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "1";

/*
***Ta reda på storhetens:s ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = TXTTYP;
    if ( (status=getidt(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();
/*
***Kolla att storheten inte ingår i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Läs text-posten och ta reda på nuvarande font.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_text(&txt,NULL,la);
    font = txt.fnt_tx;
/*
***Fråga efter ny font.
*/
    sprintf(strbuf,"%s%d  %s",iggtts(43),font,iggtts(109));
    igplma(strbuf,IG_INP);

    pmmark();
    if ( (status=genint(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket och beräkna ny font.
*/
    inevev(exnpt,&val,&valtyp);
    font = val.lit.int_va;
/*
***Ändra font i PM, GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    val.lit_type = C_INT_VA;
    val.lit.int_va = font;
    pmclie(&val,&exnpt);

    if ( v3mode & BAS_MOD ) pmchnp(idvek,PMTFONT,exnpt,&retla);
    txt.fnt_tx = font;
    DBupdate_text(&txt,NULL,la);

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igctpm()

/*      Varkon-funktion för att ändra en texts planmode.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Storheten ingår i en part.
 *
 *      (C)microform ab 1998-10-01, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    bool    end,right;
    pm_ptr  exnpt,retla,valtyp;
    int     pmode;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    PMREFVA idvek[MXINIV];
    DBText   txt;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "1";

/*
***Ta reda på storhetens:s ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = TXTTYP;
    if ( (status=getidt(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();
/*
***Kolla att storheten inte ingår i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Läs text-posten och ta reda på nuvarande pmode.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_text(&txt,NULL,la);
    pmode = txt.pmod_tx;
/*
***Fråga efter ny pmode.
*/
    sprintf(strbuf,"%s%d  %s",iggtts(43),pmode,iggtts(1631));
    igplma(strbuf,IG_INP);

    pmmark();
    if ( (status=genint(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket och beräkna ny pmode.
*/
    inevev(exnpt,&val,&valtyp);
    pmode = val.lit.int_va;
/*
***Ändra font i PM, GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    val.lit_type = C_INT_VA;
    val.lit.int_va = pmode;
    pmclie(&val,&exnpt);

    if ( v3mode & BAS_MOD ) pmchnp(idvek,PMTPMODE,exnpt,&retla);
    txt.pmod_tx = pmode;
    DBupdate_text(&txt,NULL,la);

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcdts()

/*      Varkonfunktion för att ändra ett måtts siffer-
 *      storlek.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Storheten ingår i en part.
 *              IG3812 = Måttet har inga siffror.
 *
 *      (C)microform ab 17/3/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    tbool   dauto;
    DBptr   la;
    DBetype   typ;
    DBfloat   tsiz;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBLdim   ldm;
    DBCdim   cdm;
    DBRdim   rdm;
    DBAdim   adm;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på storhetens:s ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = LDMTYP+CDMTYP+ADMTYP+RDMTYP;
    if ( (status=getidt(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();
/*
***Kolla att storheten inte ingår i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Kolla att måttet har siffror (auto on).
*/
    DBget_pointer('I',idvek,&la,&typ);

    if ( typ == LDMTYP )
      {
      DBread_ldim(&ldm,la);
      dauto = ldm.auto_ld;
      tsiz = ldm.tsiz_ld;
      }
    else if ( typ == CDMTYP )
      {
      DBread_cdim(&cdm,la);
      dauto = cdm.auto_cd;
      tsiz = cdm.tsiz_cd;
      }
    else if ( typ == RDMTYP )
      {
      DBread_rdim(&rdm,la);
      dauto = rdm.auto_rd;
      tsiz = rdm.tsiz_rd;
      }
    else
      {
      DBread_adim(&adm,la);
      dauto = adm.auto_ad;
      tsiz = adm.tsiz_ad;
      }

    if ( !dauto )
      {
      erpush("IG3812","");
      errmes();
      goto loop;
      }
/*
***Fråga efter ny sifferstorlek.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(43),tsiz,iggtts(129));
    igplma(strbuf,IG_INP);

    pmmark();
    if ( (status=genflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket och beräkna ny strecklängd.
*/
    inevev(exnpt,&val,&valtyp);

    if ( val.lit_type == C_INT_VA ) tsiz = val.lit.int_va;
    else tsiz = val.lit.float_va;
/*
***Ändra strecklängd i PM, GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);
    if ( v3mode & BAS_MOD ) pmchnp(idvek,PMDTSIZE,exnpt,&retla);

    if ( typ == LDMTYP )
      {
      ldm.tsiz_ld = tsiz;
      DBupdate_ldim(&ldm,la);
      }
    else if ( typ == CDMTYP )
      {
      cdm.tsiz_cd = tsiz;
      DBupdate_cdim(&cdm,la);
      }
    else if ( typ == RDMTYP )
      {
      rdm.tsiz_rd = tsiz;
      DBupdate_rdim(&rdm,la);
      }
    else
      {
      adm.tsiz_ad = tsiz;
      DBupdate_adim(&adm,la);
      }

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcdas()

/*      Varkonfunktion för att ändra ett måtts pil-
 *      storlek.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Storheten ingår i en part.
 *
 *      (C)microform ab 17/3/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la;
    DBetype   typ;
    DBfloat   asiz;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBLdim   ldm;
    DBCdim   cdm;
    DBRdim   rdm;
    DBAdim   adm;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på storhetens:s ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = LDMTYP+CDMTYP+ADMTYP+RDMTYP;
    if ( (status=getidt(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();
/*
***Kolla att storheten inte ingår i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Läs gammal pilstorlek.
*/
    DBget_pointer('I',idvek,&la,&typ);

    if ( typ == LDMTYP )
      {
      DBread_ldim(&ldm,la);
      asiz = ldm.asiz_ld;
      }
    else if ( typ == CDMTYP )
      {
      DBread_cdim(&cdm,la);
      asiz = cdm.asiz_cd;
      }
    else if ( typ == RDMTYP )
      {
      DBread_rdim(&rdm,la);
      asiz = rdm.asiz_rd;
      }
    else
      {
      DBread_adim(&adm,la);
      asiz = adm.asiz_ad;
      }
/*
***Fråga efter ny pilstorlek.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(43),asiz,iggtts(128));
    igplma(strbuf,IG_INP);

    pmmark();
    if ( (status=genflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket och beräkna ny strecklängd.
*/
    inevev(exnpt,&val,&valtyp);

    if ( val.lit_type == C_INT_VA ) asiz = val.lit.int_va;
    else asiz = val.lit.float_va;
/*
***Ändra strecklängd i PM, GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);
    if ( v3mode & BAS_MOD ) pmchnp(idvek,PMDASIZE,exnpt,&retla);

    if ( typ == LDMTYP )
      {
      ldm.asiz_ld = asiz;
      DBupdate_ldim(&ldm,la);
      }
    else if ( typ == CDMTYP )
      {
      cdm.asiz_cd = asiz;
      DBupdate_cdim(&cdm,la);
      }
    else if ( typ == RDMTYP )
      {
      rdm.asiz_rd = asiz;
      DBupdate_rdim(&rdm,la);
      }
    else
      {
      adm.asiz_ad = asiz;
      DBupdate_adim(&adm,la);
      }

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcdnd()

/*      Varkonfunktion för att ändra ett måtts antal
 *      decimaler.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Storheten ingår i en part.
 *              IG3822 = Måttet har inga siffror.
 *
 *      (C)microform ab 17/3/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    tbool   dauto;
    DBptr   la;
    DBetype   typ;
    short   ndig;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBLdim   ldm;
    DBCdim   cdm;
    DBRdim   rdm;
    DBAdim   adm;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på storhetens:s ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = LDMTYP+CDMTYP+ADMTYP+RDMTYP;
    if ( (status=getidt(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();
/*
***Kolla att storheten inte ingår i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Kolla att måttet har siffror (auto on).
*/
    DBget_pointer('I',idvek,&la,&typ);

    if ( typ == LDMTYP )
      {
      DBread_ldim(&ldm,la);
      dauto = ldm.auto_ld;
      ndig = ldm.ndig_ld;
      }
    else if ( typ == CDMTYP )
      {
      DBread_cdim(&cdm,la);
      dauto = cdm.auto_cd;
      ndig = cdm.ndig_cd;
      }
    else if ( typ == RDMTYP )
      {
      DBread_rdim(&rdm,la);
      dauto = rdm.auto_rd;
      ndig = rdm.ndig_rd;
      }
    else
      {
      DBread_adim(&adm,la);
      dauto = adm.auto_ad;
      ndig = adm.ndig_ad;
      }

    if ( !dauto )
      {
      erpush("IG3822","");
      errmes();
      goto loop;
      }
/*
***Fråga efter ny sifferstorlek.
*/
    sprintf(strbuf,"%s%d  %s",iggtts(43),ndig,iggtts(130));
    igplma(strbuf,IG_INP);

    pmmark();
    if ( (status=genint(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket och beräkna ny strecklängd.
*/
    inevev(exnpt,&val,&valtyp);

    ndig = val.lit.int_va;
/*
***Ändra strecklängd i PM, GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( v3mode & BAS_MOD ) pmchnp(idvek,PMDNDIG,exnpt,&retla);

    if ( typ == LDMTYP )
      {
      ldm.ndig_ld = ndig;
      DBupdate_ldim(&ldm,la);
      }
    else if ( typ == CDMTYP )
      {
      cdm.ndig_cd = ndig;
      DBupdate_cdim(&cdm,la);
      }
    else if ( typ == RDMTYP )
      {
      rdm.ndig_rd = ndig;
      DBupdate_rdim(&rdm,la);
      }
    else
      {
      adm.ndig_ad = ndig;
      DBupdate_adim(&adm,la);
      }

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcda0()

/*      Varkonfunktion för att ändra ett mått till
 *      auto av.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 18/3/88 J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcdau((short)0));
   }

/********************************************************/
/*!******************************************************/

        short igcda1()

/*      Varkonfunktion för att ändra ett mått till
 *      auto på.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 18/3/88 J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcdau((short)1));
   }

/********************************************************/
/*!******************************************************/

static short igcdau(short dauto)

/*      Ändrar auto på ett mått.
 *
 *      In: auto = 0 eller 1.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = Avsluta
 *          GOMAIN = Huvudmenyn
 *
 *      Felkod: IG3502 = Storheten ingår i en part.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    bool    end,right;
    pm_ptr  exnpt,retla;
    PMREFVA idvek[MXINIV];
    DBLdim   ldm;
    DBCdim   cdm;
    DBRdim   rdm;
    DBAdim   adm;
    PMLITVA val;

/*
***Ta reda på storhetens:s ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = LDMTYP+CDMTYP+RDMTYP+ADMTYP;
    if ( (status=getidt(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();
/*
***Kolla att storheten inte ingår i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Ändra font i PM, GM och GP.
*/
    DBget_pointer('I',idvek,&la,&typ);

    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( v3mode & BAS_MOD )
      {
      val.lit_type = C_INT_VA;
      val.lit.int_va = dauto;
      pmclie(&val,&exnpt);
      pmchnp(idvek,PMDAUTO,exnpt,&retla);
      }

    if ( typ == LDMTYP )
      {
      DBread_ldim(&ldm,la);
      ldm.auto_ld = dauto;
      DBupdate_ldim(&ldm,la);
      }
    else if ( typ == CDMTYP )
      {
      DBread_cdim(&cdm,la);
      cdm.auto_cd = dauto;
      DBupdate_cdim(&cdm,la);
      }
    else if ( typ == RDMTYP )
      {
      DBread_rdim(&rdm,la);
      rdm.auto_rd = dauto;
      DBupdate_rdim(&rdm,la);
      }
    else
      {
      DBread_adim(&adm,la);
      adm.auto_ad = dauto;
      DBupdate_adim(&adm,la);
      }


    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    igrsma();
    return(status);
  }

/********************************************************/
