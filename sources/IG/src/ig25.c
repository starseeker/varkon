/*!******************************************************************/
/*  File: ig25.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  mvengm();    Move many entities                                 */
/*  mv1gm();     Move one entity                                    */
/*  cpengm();    Copy entities                                      */
/*  mrengm();    Mirror entities                                    */
/*  roengm();    Rotate entities                                    */
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
#include "../../WP/include/WP.h"

extern short posmod;

/*!******************************************************/

        short mvengm()

/*      Varkonfunktion för att flytta storheter.
 *      Ingår bara i Rit-modulen.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      (C)microform ab 17/3/88 J. Kjellander
 *
 ******************************************************!*/

  {
    DBetype  typv[IGMAXID];
    short    idant,status;
    DBId     idmat[IGMAXID][MXINIV];
    DBVector p1,p2;

/*
***Hämta id för storheter som skall flyttas.
*/
    igptma(147,IG_MESS);
    idant = IGMAXID;
    typv[0] = POITYP+LINTYP+ARCTYP+TXTTYP+LDMTYP+CDMTYP+RDMTYP+
              ADMTYP+XHTTYP+CURTYP+CSYTYP+PRTTYP;
    status = getmid (idmat,typv,&idant);
    igrsma();
    if ( status < 0  ||  idant < 1 ) goto exit;
/*
***Bas- och delta-position.
*/
    igptma(310,IG_MESS);
    status = genpov(&p1);
    igrsma();
    if ( status < 0 ) goto exit;

    igptma(133,IG_MESS);
    status = genpov(&p2);
    igrsma();
    if ( status < 0 ) goto exit;
/*
***Flytta.
*/
    status = EXmove(idmat,idant,&p1,&p2);
/*
***Slut.
*/
exit:
    WPerhg();
    if ( status < 0 && status != REJECT  &&  status != GOMAIN ) errmes();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short mv1gm()

/*      Varkonfunktion för att flytta en storhet.
 *      Ingår bara i Rit-modulen.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      (C)microform ab 17/3/88 J. Kjellander
 *
 ******************************************************!*/

  {
    DBetype  typ;
    DBfloat    t;
    short    posorg,status;
    bool     end,right;
    DBId     id[1][MXINIV];
    DBVector p1,p2;
    DBCurve    cur;

/*
***Hämta id för storheten som skall flyttas.
*/
start:
    igptma(268,IG_MESS);
    typ = POITYP+LINTYP+ARCTYP+TXTTYP+LDMTYP+CDMTYP+RDMTYP+
          ADMTYP+CURTYP+CSYTYP;
    status = getidt((DBId *)id,&typ,&end,&right,(short)0);
    igrsma();
    if ( status < 0 ) goto exit;
/*
***Vad blir baspositionen.
*/
    switch ( typ )
      {
      case LINTYP:
      case ARCTYP:
      if ( end ) t = 1.0;
      else t = 0.0;
      break;

      case CURTYP:
      EXgtcu((DBId *)id,&cur,NULL,NULL);
      if ( end ) t = cur.ns_cu;
      else t = 0.0;
      break;

      default:
      t = 0;
      break;
      }

    EXon((DBId *)id,t,(DBfloat)0.0,&p1);
/*
***Den nya positionen.
*/
    posorg = posmod;
    posmod = 3;
    igptma(133,IG_MESS);
    status = genpov(&p2);
    igrsma();
    posmod = posorg;
    if ( status < 0 ) goto exit;
/*
***Flytta.
*/
    if ( (status=EXmove(id,(short)1,&p1,&p2)) < 0 ) goto exit;
    else goto start;
/*
***Slut.
*/
exit:
    WPerhg();
    if ( status < 0 && status != REJECT  &&  status != GOMAIN ) errmes();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short cpengm()

/*      Varkonfunktion för kopiera storheter.
 *      Ingår bara i Rit-modulen.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      (C)microform ab 17/3/88 J. Kjellander
 *
 *      11/12/89 Urspr. nivåer, J. Kjellander
 *
 ******************************************************!*/

  {
    DBetype  typv[IGMAXID];
    short    idant,status,antal;
    DBint    lantal;
    char     istr[V3STRLEN+1];
    bool     orgniv;
    DBId     idmat[IGMAXID][MXINIV];
    DBVector p1,p2;

static char dstr[V3STRLEN+1] = "1";

/*
***Hämta id för storheter som skall flyttas.
*/
    igptma(148,IG_MESS);
    idant = IGMAXID;
    typv[0] = POITYP+LINTYP+ARCTYP+TXTTYP+LDMTYP+CDMTYP+RDMTYP+
              ADMTYP+XHTTYP+CURTYP+CSYTYP+PRTTYP;
    status = getmid (idmat,typv,&idant);
    igrsma();
    if ( status < 0  ||  idant < 1 ) goto exit;
/*
***Bas- och delta-position.
*/
    igptma(310,IG_MESS);
    status = genpov(&p1);
    igrsma();
    if ( status < 0 ) goto exit;

    igptma(133,IG_MESS);
    status = genpov(&p2);
    igrsma();
    if ( status < 0 ) goto exit;
/*
***Antal kopior.
*/
    if ( (status=geninv(134,istr,dstr,&lantal)) < 0 ) goto exit;
    strcpy(dstr,istr);
    antal = (short)lantal;
/*
***Skall ursprungliga nivåer behållas ?
*/
    orgniv = igialt((short)163,(short)67,(short)68,FALSE);
/*
***Kopiera.
*/
    status = EXcopy(idmat,idant,&p1,&p2,antal,orgniv);
/*
***Slut.
*/
exit:
    WPerhg();
    if ( status < 0 && status != REJECT  &&  status != GOMAIN ) errmes();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short mrengm()

/*      Varkonfunktion för spegla storheter.
 *      Ingår bara i Rit-modulen.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      (C)microform ab 16/11/89 J. Kjellander
 *
 *      11/12/89 Urspr. nivåer, J. Kjellander
 *
 ******************************************************!*/

  {
    DBetype    typv[IGMAXID];
    short    idant,status;
    bool     orgniv;
    DBId     idmat[IGMAXID][MXINIV];
    DBVector    p1,p2;

/*
***Hämta id för storheter som skall speglas.
*/
    igptma(157,IG_MESS);
    idant = IGMAXID;
    typv[0] = POITYP+LINTYP+ARCTYP+TXTTYP+PRTTYP;
    status = getmid (idmat,typv,&idant);
    igrsma();
    if ( status < 0  ||  idant < 1 ) goto exit;
/*
***2 positioner på speglingsaxeln.
*/
    igptma(158,IG_MESS);
    status = genpov(&p1);
    igrsma();
    if ( status < 0 ) goto exit;

    igptma(159,IG_MESS);
    status = genpov(&p2);
    igrsma();
    if ( status < 0 ) goto exit;
/*
***Skall ursprungliga nivåer behållas ?
*/
    orgniv = igialt((short)163,(short)67,(short)68,FALSE);
/*
***Spegla.
*/
    status = EXmirr(idmat,idant,&p1,&p2,orgniv);
/*
***Slut.
*/
exit:
    WPerhg();
    if ( status < 0 && status != REJECT  &&  status != GOMAIN ) errmes();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short roengm()

/*      Varkonfunktion för rotera storheter.
 *      Ingår bara i Rit-modulen.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    DBetype    typv[IGMAXID];
    short    idant,status;
    double   v;
    char     istr[V3STRLEN+1];
    DBId     idmat[IGMAXID][MXINIV];
    DBVector    p;

static char dstr[V3STRLEN+1] = "0.0";
/*
***Hämta id för storheter som skall roteras.
*/
    igptma(110,IG_MESS);
    idant = IGMAXID;
    typv[0] = POITYP+LINTYP+ARCTYP+TXTTYP+LDMTYP+CDMTYP+RDMTYP+
              ADMTYP+XHTTYP+CURTYP+CSYTYP+PRTTYP;
    status = getmid (idmat,typv,&idant);
    igrsma();
    if ( status < 0  ||  idant < 1 ) goto exit;
/*
***Bas-position.
*/
    igptma(310,IG_MESS);
    status = genpov(&p);
    igrsma();
    if ( status < 0 ) goto exit;
/*
***Vinkel.
*/
    if ( (status=genflv(274,istr,dstr,&v)) < 0 ) goto exit;
    strcpy(dstr,istr);
/*
***Rotera.
*/
    status = EXrot(idmat,idant,&p,v);
/*
***Slut.
*/
exit:
    WPerhg();
    if ( status < 0 && status != REJECT  &&  status != GOMAIN ) errmes();
    return(status);
  }

/********************************************************/
