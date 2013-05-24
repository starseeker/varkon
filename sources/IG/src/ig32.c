/*!******************************************************************/
/*  File: ig32.c                                                    */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  igcarr();   Edit circle radius                                  */
/*  igcar1();   Edit crcle start angle                              */
/*  igcar2();   Edit circle end angle                               */
/*  igctxv();   Edit text angle                                     */
/*  igctxs();   Edit text string                                    */
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
#include "../../EX/include/EX.h"
#include "../../GP/include/GP.h"
#include "../../GE/include/GE.h"

extern short   v3mode;
extern GMDATA  v3dbuf;

/*!******************************************************/

        short igcarr()

/*      Ändra cirkels radie.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      Felkoder: IG3502 = Storheten ingår i en part
 *                IG3832 = Radie < 0
 *
 *      (C)microform ab 23/3/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    gmflt   radie;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    DBId    idvek[MXINIV];
    GMARC   arc;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på cirkelns ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = ARCTYP;
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
***Läs nuvarande radie ur GM.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_arc(&arc,NULL,la);
    radie = arc.r_a;
/*
***Fråga efter ny radie.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(43),radie,iggtts(248));
    igplma(strbuf,IG_INP);

    if ( (status=genflv(0,istr,dstr,&radie)) < 0 ) goto exit;

    strcpy(dstr,istr);
    igrsma();

    if ( radie <= 0.0 )
      {
      erpush("IG3832","");
      errmes();
      goto loop;
      }
/*
***Ändra radie i GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);
    arc.r_a = radie;
    DBupdate_arc(&arc,NULL,la);
    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    gphgal(0);
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcar1()

/*      Ändra cirkels startvinkel.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      Felkoder: IG3502 = Storheten ingår i en part
 *                IG3842 = V1 > V2
 *
 *      (C)microform ab 23/3/88 J. Kjellander
 *
 *      7/12/89  GE312(), J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    gmflt   v1,v2;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    DBId    idvek[MXINIV];
    GMARC   arc;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på cirkelns ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = ARCTYP;
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
***Läs nuvarande vinkel ur GM.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_arc(&arc,NULL,la);
    v1 = arc.v1_a;
    v2 = arc.v2_a;
/*
***Fråga efter ny vinkel.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(43),v1,iggtts(248));
    igplma(strbuf,IG_INP);

    if ( (status=genflv(0,istr,dstr,&v1)) < 0 ) goto exit;

    strcpy(dstr,istr);
    igrsma();
/*
***Kolla att v1 <> v2.
*/
    if ( GE312(&v1,&v2) < 0 )
      {
      erpush("IG3842","");
      errmes();
      goto loop;
      }
/*
***Ändra vinkel i GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);
    arc.v1_a = v1;
    DBupdate_arc(&arc,NULL,la);
    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    gphgal(0);
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcar2()

/*      Ändra cirkels slutvinkel.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      Felkoder: IG3502 = Storheten ingår i en part
 *                IG3832 = Radie < 0
 *
 *      (C)microform ab 23/3/88 J. Kjellander
 *
 *      7/12/89  GE312(), J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    gmflt   v1,v2;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    DBId    idvek[MXINIV];
    GMARC   arc;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på cirkelns ID.
*/
loop:
    igptma(268,IG_MESS);
    typ = ARCTYP;
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
***Läs nuvarande vinkel ur GM.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_arc(&arc,NULL,la);
    v1 = arc.v1_a;
    v2 = arc.v2_a;
/*
***Fråga efter ny vinkel.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(43),v2,iggtts(248));
    igplma(strbuf,IG_INP);

    if ( (status=genflv(0,istr,dstr,&v2)) < 0 ) goto exit;

    strcpy(dstr,istr);
    igrsma();
/*
***Kolla att v1 <> v2.
*/
    if ( GE312(&v1,&v2) < 0 )
      {
      erpush("IG3842","");
      errmes();
      goto loop;
      }
/*
***Ändra vinkel i GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);
    arc.v2_a = v2;
    DBupdate_arc(&arc,NULL,la);
    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    gphgal(0);
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igctxv()

/*      Ändra texts vinkel.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      Felkoder: IG3502 = Storheten ingår i en part
 *
 *      (C)microform ab 23/3/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    gmflt   tv;
    bool    end,right;
    char    str[V3STRLEN+1];
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    DBId    idvek[MXINIV];
    GMTXT   txt;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på textens ID.
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
***Läs nuvarande vinkel ur GM.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_text(&txt,str,la);
    tv = txt.v_tx;
/*
***Fråga efter ny vinkel.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(43),tv,iggtts(248));
    igplma(strbuf,IG_INP);

    if ( (status=genflv(0,istr,dstr,&tv)) < 0 ) goto exit;

    strcpy(dstr,istr);
    igrsma();
/*
***Ändra vinkel i GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);
    txt.v_tx = tv;
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
    gphgal(0);
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igctxs()

/*      Ändra texts text.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      Felkoder: IG3502 = Storheten ingår i en part
 *
 *      (C)microform ab 23/3/88 J. Kjellander
 *
 *      19/3/89 Default=gamla texten, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    char    str[V3STRLEN+1];
    char    dstr[V3STRLEN+1];
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    bool    end,right;
    DBId    idvek[MXINIV];
    GMTXT   txt;

/*
***Ta reda på textens ID.
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
***Läs nuvarande text ur GM.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_text(&txt,str,la);
/*
***Fråga efter ny text.
*/
    sprintf(strbuf,"%s%s  %s",iggtts(43),str,iggtts(248));
    igplma(strbuf,IG_INP);
    strcpy(dstr,str);
    if ( (status=genstv(0,istr,dstr,str)) < 0 ) goto exit;
    igrsma();
/*
***Ändra text i GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);
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
    gphgal(0);
    igrsma();
    return(status);
  }

/********************************************************/
