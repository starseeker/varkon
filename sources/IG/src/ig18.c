/*!******************************************************************/
/*  File: ig18.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*    lstniv();   List named levels                                 */
/*    namniv();   Name level                                        */
/*    delniv();   Delete level name                                 */
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
#include <string.h>

extern tbool  nivtb1[];
extern NIVNAM nivtb2[];
extern V2NAPA defnap;

/*!******************************************************/

        short lstniv()

/*      Varkonfunktion för att lista namngivna nivåer.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = Ok.
 *          GOMAIN = Huvudmenyn.
 *
 *      (C)microform ab 22/1/85 J. Kjellander
 *
 *      13/9/85  Utskrifter, R. Svedin
 *      16/10/85 Aktuell nivå tänd/släckt, j: Kjellander
 *      10/11/85 Aktuell nivå ej def, J. Kjellander
 *      30/9/86  Ny nivåhantering  R. Svedin
 *      6/10/86  GOMAIN, J. Kjellander
 *      18/4/87  REJECT, J. Kjellander
 *
 ******************************************************!*/

  {
    short i,status;
    char linbuf[80];

    iginla(iggtts(47));

    for ( i=0; i<NT2SIZ; ++i)
      {
      if ( nivtb2[i].nam[0] != '\0' )
        {
        sprintf(linbuf,"  %-9d %-15s  ",nivtb2[i].num,nivtb2[i].nam);
        if ( nivtb1[nivtb2[i].num] ) strcat(linbuf,iggtts(226));
        else                 strcat(linbuf,iggtts(225));
        if ( nivtb2[i].num == defnap.level ) strcat(linbuf,iggtts(224));
        if ( (status=igalla(linbuf,(short)1)) < 0 ) return(status);
        }
      }

    return(igexla());
  }

/********************************************************/
/*!******************************************************/

        short namniv()

/*      Varkonfunktion för att namnge nivå.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3222 = Max antal nivåer namngivna
 *              IG3212 = Otillåtet nivånummer
 *              IG3242 = Nivån är redan namngiven
 *
 *      (C)microform ab 22/1/85 J. Kjellander
 *
 *      13/9/85  Utskrifter, R. Svedin 
 *      30/9/86  Ny nivåhantering, R. Svedin
 *      6/10/86  GOMAIN, B. Doverud
 *      9/19/86  IG3242, J. Kjellander
 *
 ******************************************************!*/

  {
    char   namn[NIVNLN+1];
    short  i,j,status=0;
    int    ival;

/*
***Leta upp första lediga plats i nivtb2.
*/
    for ( i = 0; i < NT2SIZ; ++i )
      {
      if ( nivtb2[i].nam[0] == '\0' )
        {
/*
***Läs in nivå-nummer.
*/
        igptma(227,IG_INP);
        if ( (status=igsiip(iggtts(319),&ival)) < 0 ) goto exit;
        if ( ival < 0 || ival > NT1SIZ-1) goto error1;
        igrsma();
/*
***Kolla om nivån redan är namngiven.
*/
        for ( j=0; j<NT2SIZ; ++j )
          {
          if ( nivtb2[j].nam[0] != '\0' && nivtb2[j].num == ival )
             goto error3;
          }
/*
***Läs in nivånamn.
*/
        igptma(14,IG_INP);
        if ( (status=igssip(iggtts(267),namn,"",NIVNLN)) < 0 ) goto exit;
        
        nivtb2[i].num = ival;
        strcpy(nivtb2[i].nam,namn);    
        goto end;
        }
      }
/*
***Ledig plats saknas ?
*/
end:
      if ( i == NT2SIZ ) goto error2;

exit:
    igrsma();
    return(status);
/*
***Felutgångar
*/
error1:
      erpush("IG3212","");
      goto errend;

error2:
      erpush("IG3222","");
      goto errend;

error3:
      erpush("IG3242","");

errend:
      errmes();
      goto exit;
  }

/********************************************************/
/*!******************************************************/

        short delniv()

/*      Varkonfunktion för att ta bort nivå.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3202 = Nivån har inget namn
 *              IG3212 = Otillåtet nivånummer
 *
 *      (C)microform ab 22/1/85 J. Kjellander
 *
 *      13/9/85  Utskrifter        R. Svedin 
 *      30/9/86  Ny nivåhantering  R. Svedin
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short i,status;
    int   ival;

/*
***Läs in nivå-nummer.
*/
    igptma(227,IG_INP);
    if ( (status=igsiip(iggtts(319),&ival)) < 0 ) goto exit;
    if ( ival < 0 || ival > NT1SIZ-1) goto error1;
/*
***Leta upp nivån i nivtb2.
*/
    for ( i = 0 ; i < NT2SIZ ; ++i)
      {
      if ( nivtb2[i].num == ival ) 
        {
        nivtb2[i].nam[0] = '\0';
        goto end;
        }
      }
/*
***Nivån finns ej ?
*/
end:
      if ( i == NT2SIZ ) goto error2;

exit:
    igrsma();
    return(status);
/*
***Felutgångar
*/
error1:
      erpush("IG3212","");
      goto errend;

error2:
      erpush("IG3202","");

errend:
      errmes();
      goto exit;
  }

/********************************************************/
