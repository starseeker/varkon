/*!******************************************************************/
/*  igview.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   IGcnog();   Set curve accuracy                                 */
/*   IGrenw();   Dynamic shading                                    */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
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
#include <string.h>

extern MNUALT  smbind[];
extern short   modtyp,mant,mstack[];

/*!******************************************************/

        short IGcnog()

/*      Varkon-funktion för ändring av kurvnoggrannhet.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      (C)microform ab 15/2/84 J. Kjellander
 *
 *      18/10/85 Minvärde = 0.001 J. Kjellander
 *      6/10/86 GOMAIN, J. Kjellander
 *      39/9/87 IGcflv, J. Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    double dblval;
    char   knstr[V3STRLEN+1];
    char   istr[V3STRLEN+1];

    static char dstr[V3STRLEN+1] = "1.0";

    WPget_cacc(&dblval);
    sprintf(knstr,"%s%g  %s",IGgtts(247),dblval,IGgtts(248));
    IGplma(knstr,IG_INP);

loop:
    if ( (status=IGcflv(0,istr,dstr,&dblval)) < 0 ) goto end;
    strcpy(dstr,istr);

    if ( EXcavi(dblval) < 0 ) 
      {
      errmes();
      goto loop;
      }
    else
      {
      WPrepaint_GWIN(GWIN_ALL);
      WPrepaint_RWIN(RWIN_ALL,FALSE);
      }

end:
    IGrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGrenw()

/*      Varkon-funktion för renderingsfönstret.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1997-12-21 J. Kjellander
 *
 ******************************************************!*/

  {

#ifdef UNIX
extern short WPrenw();

   if ( WPrenw() < 0 ) errmes();
#endif

#ifdef WIN32
extern short msrenw();

   if ( msrenw() < 0 ) errmes();
#endif


   return(0);
  }

/********************************************************/
