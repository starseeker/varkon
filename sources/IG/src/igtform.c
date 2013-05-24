/*!******************************************************************/
/*  igtform.c                                                       */
/*  =========                                                       */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGtfmo();    Genererate tform_move... statement                 */
/*  IGtfro();    Genererate tform_rotl... statement                 */
/*  IGtfmi();    Genererate tform_mirr... statement                 */
/*  IGtcpy();    Genererate tcopy...      statement                 */
/*  IGmven();    Move many entities                                 */
/*  IGmv1();     Move one entity                                    */
/*  IGcpy1();    Copy entities                                      */
/*  IGmir1();    Mirror entities                                    */
/*  IGrot1();    Rotate entities                                    */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://varkon.sourceforge.net                             */
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
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/IG.h"
#include "../../WP/include/WP.h"
#include "../../EX/include/EX.h"

extern int posmode;

/*!******************************************************/

       short IGtfmo()

/*      Huvudrutin f�r tform_move...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa TFORM_MOVE  sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text str�ng nummer G. Liden
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,retla;
/*
***Skapa start punkt.
*/
    if ( (status=IGcpos( 525,&exnpt1)) < 0 ) goto exit;
/*
***Skapa slut punkt.
*/
    if ( (status=IGcpos( 524,&exnpt2)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("TFORM_MOVE",valparam) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }
exit:
    WPerhg();
    return(status);
  }
/********************************************************/
/*!******************************************************/

       short IGtfro()

/*      Huvudrutin f�r tform_rotl...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa TFORM_ROTL  sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text str�ng nummer G. Liden
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,exnpt3,retla;
    char        istr[V3STRLEN+1];
    static char rotstr[V3STRLEN+1] ="0.0";
/*
***Skapa start punkt rotationslinje.
*/
    if ( (status=IGcpos( 523,&exnpt1)) < 0 ) goto exit;
/*
***Skapa slut punkt rotationslinje.
*/
    if ( (status=IGcpos( 522,&exnpt2)) < 0 ) goto exit;
/*
***Skapa rotationsvinkel.
*/
    if ( (status=IGcflt(17,rotstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(rotstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,      &retla,  &dummy);
    pmtcon(exnpt3,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("TFORM_ROTL",valparam) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }
exit:
    WPerhg();
    return(status);
  }
/********************************************************/
/*!******************************************************/

       short IGtfmi()

/*      Huvudrutin f�r tform_mirr...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa TFORM_MIRR  sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text str�ng nummer G. Liden
 *      1998-04-16 t554-556, J.Kjellander
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,exnpt3,retla;
/*
***Skapa punkt 1 i speglingsplan.
*/
    if ( (status=IGcpos( 554,&exnpt1)) < 0 ) goto exit;
/*
***Skapa punkt 2 i speglingsplan.
*/
    if ( (status=IGcpos( 555,&exnpt2)) < 0 ) goto exit;
/*
***Skapa punkt 3 i speglingsplan.
*/
    if ( (status=IGcpos( 556,&exnpt3)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,      &retla,  &dummy);
    pmtcon(exnpt3,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("TFORM_MIRR",valparam) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }
exit:
    WPerhg();
    return(status);
  }
/********************************************************/
/*!******************************************************/

       short IGtcpy()

/*      Huvudrutin f�r tcopy...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa TCOPY  sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text str�ng nummer G. Liden
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,retla;
    DBetype       typ;
    bool        end,right;
/*
***Skapa referens till storhet.
*/
    typ = ALLTYP;
    if ( (status=IGcref (268,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa referens till transformationsmatris.
*/
    typ = TRFTYP;
    if ( (status=IGcref ( 520,&typ,&exnpt2,&end,&right)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("TCOPY",valparam) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }
exit:
    WPerhg();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGmven()

/*      Varkonfunktion f�r att flytta storheter.
 *      Ing�r bara i Rit-modulen.
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
    short    status;
    int      idant;
    DBId     idmat[IGMAXID][MXINIV];
    DBVector p1,p2;

/*
***H�mta id f�r storheter som skall flyttas.
*/
    IGptma(147,IG_MESS);
    idant = IGMAXID;
    typv[0] = POITYP+LINTYP+ARCTYP+TXTTYP+LDMTYP+CDMTYP+RDMTYP+
              ADMTYP+XHTTYP+CURTYP+CSYTYP+PRTTYP;
    status = IGgmid (idmat,typv,&idant);
    IGrsma();
    if ( status < 0  ||  idant < 1 ) goto exit;
/*
***Bas- och delta-position.
*/
    IGptma(310,IG_MESS);
    status = IGcpov(&p1);
    IGrsma();
    if ( status < 0 ) goto exit;

    IGptma(133,IG_MESS);
    status = IGcpov(&p2);
    IGrsma();
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

        short IGmv1()

/*      Varkonfunktion f�r att flytta en storhet.
 *      Ing�r bara i Rit-modulen.
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
    DBfloat  t;
    short    status;
    int      posorg;
    bool     end,right;
    DBId     id[1][MXINIV];
    DBVector p1,p2;
    DBCurve  cur;

/*
***H�mta id f�r storheten som skall flyttas.
*/
start:
    IGptma(268,IG_MESS);
    typ = POITYP+LINTYP+ARCTYP+TXTTYP+LDMTYP+CDMTYP+RDMTYP+
          ADMTYP+CURTYP+CSYTYP;
    status = IGgsid((DBId *)id,&typ,&end,&right,(short)0);
    IGrsma();
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
    posorg = posmode;
    posmode = 2;
    IGptma(133,IG_MESS);
    status = IGcpov(&p2);
    IGrsma();
    posmode = posorg;
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

        short IGcpy1()

/*      Varkonfunktion f�r kopiera storheter.
 *      Ing�r bara i Rit-modulen.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      (C)microform ab 17/3/88 J. Kjellander
 *
 *      11/12/89 Urspr. niv�er, J. Kjellander
 *
 ******************************************************!*/

  {
    DBetype  typv[IGMAXID];
    short    status,antal;
    DBint    idant,lantal;
    char     istr[V3STRLEN+1];
    bool     orgniv;
    DBId     idmat[IGMAXID][MXINIV];
    DBVector p1,p2;

static char dstr[V3STRLEN+1] = "1";

/*
***H�mta id f�r storheter som skall flyttas.
*/
    IGptma(148,IG_MESS);
    idant = IGMAXID;
    typv[0] = POITYP+LINTYP+ARCTYP+TXTTYP+LDMTYP+CDMTYP+RDMTYP+
              ADMTYP+XHTTYP+CURTYP+CSYTYP+PRTTYP;
    status = IGgmid (idmat,typv,&idant);
    IGrsma();
    if ( status < 0  ||  idant < 1 ) goto exit;
/*
***Bas- och delta-position.
*/
    IGptma(310,IG_MESS);
    status = IGcpov(&p1);
    IGrsma();
    if ( status < 0 ) goto exit;

    IGptma(133,IG_MESS);
    status = IGcpov(&p2);
    IGrsma();
    if ( status < 0 ) goto exit;
/*
***Antal kopior.
*/
    if ( (status=IGcinv(134,istr,dstr,&lantal)) < 0 ) goto exit;
    strcpy(dstr,istr);
    antal = (short)lantal;
/*
***Skall ursprungliga niv�er beh�llas ?
*/
    orgniv = IGialt((short)163,(short)67,(short)68,FALSE);
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

        short IGmir1()

/*      Varkonfunktion f�r spegla storheter.
 *      Ing�r bara i Rit-modulen.
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
 *      11/12/89 Urspr. niv�er, J. Kjellander
 *
 ******************************************************!*/

  {
    DBetype  typv[IGMAXID];
    short    status;
    int      idant;
    bool     orgniv;
    DBId     idmat[IGMAXID][MXINIV];
    DBVector p1,p2;

/*
***H�mta id f�r storheter som skall speglas.
*/
    IGptma(157,IG_MESS);
    idant = IGMAXID;
    typv[0] = POITYP+LINTYP+ARCTYP+TXTTYP+PRTTYP;
    status = IGgmid (idmat,typv,&idant);
    IGrsma();
    if ( status < 0  ||  idant < 1 ) goto exit;
/*
***2 positioner p� speglingsaxeln.
*/
    IGptma(158,IG_MESS);
    status = IGcpov(&p1);
    IGrsma();
    if ( status < 0 ) goto exit;

    IGptma(159,IG_MESS);
    status = IGcpov(&p2);
    IGrsma();
    if ( status < 0 ) goto exit;
/*
***Skall ursprungliga niv�er beh�llas ?
*/
    orgniv = IGialt((short)163,(short)67,(short)68,FALSE);
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

        short IGrot1()

/*      Varkonfunktion f�r rotera storheter.
 *      Ing�r bara i Rit-modulen.
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
    DBetype  typv[IGMAXID];
    short    status;
    int      idant;
    double   v;
    char     istr[V3STRLEN+1];
    DBId     idmat[IGMAXID][MXINIV];
    DBVector p;

static char dstr[V3STRLEN+1] = "0.0";
/*
***H�mta id f�r storheter som skall roteras.
*/
    IGptma(110,IG_MESS);
    idant = IGMAXID;
    typv[0] = POITYP+LINTYP+ARCTYP+TXTTYP+LDMTYP+CDMTYP+RDMTYP+
              ADMTYP+XHTTYP+CURTYP+CSYTYP+PRTTYP;
    status = IGgmid (idmat,typv,&idant);
    IGrsma();
    if ( status < 0  ||  idant < 1 ) goto exit;
/*
***Bas-position.
*/
    IGptma(310,IG_MESS);
    status = IGcpov(&p);
    IGrsma();
    if ( status < 0 ) goto exit;
/*
***Vinkel.
*/
    if ( (status=IGcflv(274,istr,dstr,&v)) < 0 ) goto exit;
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
